#!/usr/bin/env bash
# Render the methods companion for review / GitHub Pages. Mirrors the data note's pipeline
# exactly (docs/notes/data-note/render.sh): strips claim/editorial HTML comments, then
# pandoc -> standalone styled HTML with the same CSS and the same KaTeX pass, plus a PDF build
# product via Pandoc -> LaTeX when a unicode-capable engine is present. The companion is prose
# and tables with no math, so the KaTeX pass no-ops (confirmed, not assumed: the rendered HTML
# carries no `katex` spans — see the render check). The HTML is a committed build product
# (regenerable from methods-companion.md); the PDF is a local build product.
# Requires: pandoc, python3 (HTML); a LaTeX engine (tectonic/xelatex/lualatex) for the PDF.
# Run from anywhere: docs/notes/methods-companion/render.sh
set -euo pipefail
cd "$(dirname "$0")"

TITLE="What it took: methods, failures, and costs of an AI-driven replication in computational number theory"

STYLE="$(mktemp)"
cat > "$STYLE" <<'CSS'
<style>
  body{max-width:820px;margin:2rem auto;padding:0 1.1rem;
       font-family:-apple-system,BlinkMacSystemFont,system-ui,"Segoe UI",sans-serif;
       line-height:1.55;color:#1b1b1b}
  h1{font-size:1.7rem;line-height:1.25} h2{margin-top:2rem;border-bottom:1px solid #e3e3e3;padding-bottom:.2rem}
  h3{margin-top:1.4rem}
  code{background:#f3f3f3;padding:.1em .3em;border-radius:3px;font-size:.92em}
  pre{background:#f7f7f7;padding:.8rem 1rem;overflow-x:auto;border-radius:6px;border:1px solid #ececec}
  pre code{background:none;padding:0}
  table{border-collapse:collapse;margin:1.1rem 0;font-size:.95em;display:block;overflow-x:auto}
  th,td{border:1px solid #d0d0d0;padding:.35em .7em;text-align:left}
  th{background:#f5f5f5}
  blockquote{border-left:3px solid #c9c9c9;margin:1.1rem 0;padding:.2rem .7rem;color:#3a3a3a;background:#fafafa}
  a{color:#0b62c4}
  hr{border:none;border-top:1px solid #e3e3e3;margin:2rem 0}
  /* wide display formulas scroll inside their own box, never the page body */
  .katex-display{overflow-x:auto;overflow-y:hidden;padding:.3rem 0}
</style>
CSS

# strip <!-- ... --> (incl. multi-line) so the rendered source carries no claim/editorial markers.
# The companion currently has none; the strip is kept for parity with the data-note pipeline.
STRIPPED="$(mktemp)"
python3 -c "import re,sys; sys.stdout.write(re.sub(r'<!--.*?-->','',open('methods-companion.md',encoding='utf-8').read(),flags=re.DOTALL))" > "$STRIPPED"

# Leak-guard: no comment delimiter may survive the strip (a malformed comment leaks its tail as
# visible text). Fail loudly so a reader never sees a dangling marker.
if grep -Eq -- '<!--|-->' "$STRIPPED"; then
  echo "ERROR: a comment fragment survived the strip in methods-companion.md (a reader would see it)." >&2
  grep -nE -- '<!--|-->' "$STRIPPED" | head >&2
  rm -f "$STYLE" "$STRIPPED"; exit 1
fi

# HTML: --katex renders $…$/$$…$$ as KaTeX. This document has no math, so the pass no-ops; the
# render check below asserts no `katex` span survives (dollar signs in the cost tables must
# render as literal '$', not open math). pagetitle sets the browser-tab <title> without emitting
# pandoc's own title block, so the document's own `# ` H1 is the single visible title.
pandoc -f markdown -s --katex --metadata pagetitle="$TITLE" -H "$STYLE" -o methods-companion.html "$STRIPPED"

# Confirm-not-assume: no math was produced (no KaTeX span), and the dollar amounts survived.
if grep -q 'class="katex' methods-companion.html; then
  echo "ERROR: KaTeX spans appeared in a no-math document — a '\$' was parsed as math. Escape it (\\\$) in the source." >&2
  rm -f "$STYLE" "$STRIPPED"; exit 1
fi
echo "wrote $(pwd)/methods-companion.html (KaTeX pass no-op confirmed: no katex spans)"

# PDF build product via Pandoc -> LaTeX (unicode-capable engine required; skip cleanly if none).
PDF_ENGINE=""
for e in tectonic xelatex lualatex; do
  if command -v "$e" >/dev/null 2>&1; then PDF_ENGINE="$e"; break; fi
done
if [ -n "$PDF_ENGINE" ]; then
  # Drop the leading `# ` title line so the title comes from \maketitle (metadata) exactly once;
  # promote the note's `##` sections to \section.
  PDFSRC="$(mktemp)"
  python3 -c "import re,sys; sys.stdout.write(re.sub(r'^# .*\n','',open('$STRIPPED',encoding='utf-8').read(),count=1,flags=re.M))" > "$PDFSRC"

  LATEXHDR="$(mktemp)"
  cat > "$LATEXHDR" <<'TEX'
\usepackage{fontspec}
\usepackage{microtype}
\emergencystretch=3em
\hbadness=2000
\usepackage{fvextra}
\RecustomVerbatimEnvironment{verbatim}{Verbatim}{breaklines=true,breakanywhere=true,fontsize=\small}
\usepackage{seqsplit}
\let\oldtexttt\texttt
\renewcommand{\texttt}[1]{{\oldtexttt{\seqsplit{#1}}}}
% No section numbering — the companion carries its own "1./2." heading prefixes.
\setcounter{secnumdepth}{0}
\renewenvironment{quote}{\list{}{\leftmargin=1.2em \rightmargin=0em}\item\relax}{\endlist}
\usepackage{titling}
\pretitle{\begin{center}\LARGE\bfseries\hyphenpenalty=10000\exhyphenpenalty=10000 }
\posttitle{\par\end{center}\vskip 0.5em}
TEX
  # --lua-filter drops pandoc's forced-equal proportional table columns for natural booktabs
  # columns, reusing the data note's filter.
  pandoc -f markdown --pdf-engine="$PDF_ENGINE" \
    --shift-heading-level-by=-1 --metadata title="$TITLE" \
    --lua-filter=../data-note/pdf-tables.lua \
    -V geometry:margin=1in -H "$LATEXHDR" -o methods-companion.pdf "$PDFSRC"
  echo "wrote $(pwd)/methods-companion.pdf (engine: $PDF_ENGINE)"
  rm -f "$LATEXHDR" "$PDFSRC"
else
  echo "[SKIP] PDF: no unicode-capable LaTeX engine (tectonic/xelatex/lualatex) on PATH — HTML only."
fi

rm -f "$STYLE" "$STRIPPED"
