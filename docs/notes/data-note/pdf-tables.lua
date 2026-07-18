-- pandoc Lua filter (PDF/LaTeX path of render.sh only).
-- Pandoc renders pipe tables as a longtable with forced-EQUAL proportional p{} column
-- widths (e.g. 0.1667 each), which wraps wide headers ("first zero u (dev)"), wastes
-- space on the numeric columns, and splits a short table across pages. Dropping the
-- explicit widths makes pandoc emit natural, content-sized columns (@{}llllll@{}) under
-- the same booktabs rules — the "booktabs + fixed column spec" the note wants. A 4-row
-- table then fits on one page (longtable only breaks when it overflows one).
function Table(t)
  for i = 1, #t.colspecs do
    t.colspecs[i][2] = nil  -- width -> ColWidthDefault (natural width)
  end
  return t
end
