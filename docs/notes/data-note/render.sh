#!/usr/bin/env bash
# Render the data note for review / GitHub Pages (venue amendment 2026-07-17).
# Strips the claim:/editorial HTML comments, then pandoc -> standalone styled HTML (KaTeX math)
# — the marker-free artifact a reader sees, not the marker-riddled Markdown source. Also builds
# a PDF build product via Pandoc -> LaTeX when a unicode-capable engine is present. The HTML is a
# build product (regenerable from data-note.md); committed so a reviewer can open it directly.
# Requires: pandoc, python3 (HTML); a LaTeX engine (tectonic/xelatex/lualatex) for the PDF.
# Run from anywhere: docs/notes/data-note/render.sh
set -euo pipefail
cd "$(dirname "$0")"

TITLE="A pre-registered replication of the Sawin–Sutherland height-ordered murmuration"

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
  blockquote{border-left:3px solid #c9c9c9;margin:1.1rem 0;padding:.2rem 1rem;color:#3a3a3a;background:#fafafa}
  a{color:#0b62c4}
  hr{border:none;border-top:1px solid #e3e3e3;margin:2rem 0}
  /* wide display formulas scroll inside their own box, never the page body */
  .katex-display{overflow-x:auto;overflow-y:hidden;padding:.3rem 0}
</style>
CSS

# strip <!-- ... --> (incl. multi-line) so the rendered source carries no claim/editorial markers
STRIPPED="$(mktemp)"
python3 -c "import re,sys; sys.stdout.write(re.sub(r'<!--.*?-->','',open('data-note.md',encoding='utf-8').read(),flags=re.DOTALL))" > "$STRIPPED"

# Leak-guard: no comment delimiter may survive the strip. A comment whose body embeds a literal
# <!-- ... --> (malformed HTML — a comment can't contain '--') breaks the non-greedy strip and
# leaks its tail as visible text; fail loudly so a reader never sees a dangling marker.
if grep -Eq -- '<!--|-->' "$STRIPPED"; then
  echo "ERROR: a comment fragment survived the strip in data-note.md (a reader would see it)." >&2
  echo "Fix the SOURCE: an editorial comment must not embed a literal <!-- ... --> or a bare -->:" >&2
  grep -nE -- '<!--|-->' "$STRIPPED" | head >&2
  rm -f "$STYLE" "$STRIPPED"; exit 1
fi

# HTML: --katex renders $…$/$$…$$ as KaTeX (math notation, not ASCII code fences). pagetitle (not
# title) sets the browser-tab <title> WITHOUT rendering pandoc's own title block, so the
# document's own `# ` H1 is the single visible title (no duplicate).
pandoc -f markdown -s --katex --metadata pagetitle="$TITLE" -H "$STYLE" -o data-note.html "$STRIPPED"
echo "wrote $(pwd)/data-note.html"

# PDF build product via Pandoc -> LaTeX. pdflatex can't handle this note's unicode (√, ε, 𝔽, Ш,
# superscripts), so require a unicode-capable engine; skip cleanly (not a failure) if none is
# installed — the PDF is a Phase-3 build product, the HTML is the primary artifact.
PDF_ENGINE=""
for e in tectonic xelatex lualatex; do
  if command -v "$e" >/dev/null 2>&1; then PDF_ENGINE="$e"; break; fi
done
if [ -n "$PDF_ENGINE" ]; then
  # The main TeX font (Latin Modern) lacks Cyrillic Ш (Tate–Shafarevich); map just that glyph to a
  # Cyrillic-capable font IF one is installed, so the PDF always builds — it degrades to a "missing
  # character" warning (never a halt) where the font is absent. STIX Two Text ships with macOS and
  # is freely available; \IfFontExistsTF keeps the fallback optional.
  LATEXHDR="$(mktemp)"
  cat > "$LATEXHDR" <<'TEX'
\usepackage{fontspec}
\usepackage{newunicodechar}
\IfFontExistsTF{STIX Two Text}{%
  \newfontfamily\cyrfont{STIX Two Text}%
  \newunicodechar{Ш}{{\cyrfont Ш}}%
}{}
% --listings renders code blocks via the listings package; wrap long lines (the shell
% recipe) so a verbatim line never runs into the margin (LaTeX verbatim does not wrap).
\lstset{breaklines=true,breakatwhitespace=false,basicstyle=\small\ttfamily,columns=fullflexible}
% No metadata `title` (would add \maketitle ON TOP of the document's own `# ` H1 — a
% duplicate). The body H1 is the single title, as in the HTML. secnumdepth 0 suppresses
% LaTeX section numbering so the note's literal "1./2." heading prefixes are not doubled.
\setcounter{secnumdepth}{0}
TEX
  pandoc -f markdown --pdf-engine="$PDF_ENGINE" --listings -V geometry:margin=1in \
    -H "$LATEXHDR" -V title-meta="$TITLE" -o data-note.pdf "$STRIPPED"
  echo "wrote $(pwd)/data-note.pdf (engine: $PDF_ENGINE)"
  rm -f "$LATEXHDR"
else
  echo "[SKIP] PDF: no unicode-capable LaTeX engine (tectonic/xelatex/lualatex) on PATH — HTML only."
fi

rm -f "$STYLE" "$STRIPPED"
