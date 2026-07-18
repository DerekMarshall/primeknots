#!/usr/bin/env bash
# Rough render of the data note for review / GitHub Pages (venue amendment 2026-07-17).
# Strips the claim:/editorial HTML comments, then pandoc -> standalone styled HTML — the
# marker-free artifact a reader sees, not the marker-riddled Markdown source. The HTML is a
# build product (regenerable from data-note.md); committed so a reviewer can open it directly.
# Requires: pandoc, python3. Run from anywhere: docs/notes/data-note/render.sh
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
</style>
CSS

# strip <!-- ... --> (incl. multi-line) so the rendered source carries no claim/editorial markers
python3 -c "import re,sys; sys.stdout.write(re.sub(r'<!--.*?-->','',open('data-note.md',encoding='utf-8').read(),flags=re.DOTALL))" \
  | pandoc -f markdown -s --metadata title="$TITLE" -H "$STYLE" -o data-note.html

rm -f "$STYLE"
echo "wrote $(pwd)/data-note.html"
