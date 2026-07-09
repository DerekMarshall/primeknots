# docs/papers/

Source papers for the stages, referenced normatively from `docs/RESEARCH.md`.
The PDFs themselves are **not committed** (third-party copyright); they are
gitignored. Fetch them from the sources below into this directory with the
filenames the notes expect.

| Filename (expected) | Work | Source |
|---|---|---|
| `ckkppy-1706.03336.pdf` | Chung, Kim, Kim, Pappas, Park, Yoo — *Abelian arithmetic Chern–Simons theory and arithmetic linking numbers* (Stage 4 primary) | arXiv:1706.03336 — https://arxiv.org/abs/1706.03336 |
| `cs2-1609.03012.pdf` | Chung, Kim, Kim, Park, Yoo — *Arithmetic Chern–Simons Theory II* | arXiv:1609.03012 — https://arxiv.org/abs/1609.03012 |
| `hirano-1911.12964.pdf` | H. Hirano — *On mod 2 arithmetic Dijkgraaf–Witten invariants for certain real quadratic number fields* (Stage 4 route) | arXiv:1911.12964 — https://arxiv.org/abs/1911.12964 |
| `stevenhagen-1806.06250.pdf` | P. Stevenhagen — *Rédei reciprocity, governing fields and negative Pell* (Stage 2 normative) | arXiv:1806.06250 — https://arxiv.org/abs/1806.06250 |
| `corsman-thesis.pdf` | J. Corsman — *Rédei symbols and governing fields* (Stage 2 implementation reference) | PhD thesis, McMaster University (2007); via McMaster's MacSphere institutional repository |

Other normative sources are books, not distributable here — see `docs/RESEARCH.md`
§10 for the full reference list: Edwards, *Riemann's Zeta Function* [E74]
(Stage 5 Riemann–Siegel); Parry–Pollicott [PP90] (Stage 5 model flow); Cohen
[Coh93]; Morishita [M12].

Odlyzko's zero table (Stage 5 oracle) lives under `data/odlyzko/` and is fetched
by `oracle/fetch_odlyzko.py` against a committed checksum manifest.

## Murmurations (M-stages), from `docs/RESEARCH-M.md`

Fetched by `oracle/fetch_papers_m.py` (title-page verified; manifest written to
`docs/papers/MANIFEST-M.json`). PDFs gitignored as above.

| Filename (expected) | Work | Source |
|---|---|---|
| `hlop-2204.10140.pdf` | He, Lee, Oliver, Pozdnyakov — *Murmurations of elliptic curves* [HLOP22] (M1 normative) | arXiv:2204.10140 |
| `lop-2307.00256.pdf` | Lee, Oliver, Pozdnyakov — *Murmurations of Dirichlet characters* [LOP23] (M2 normative) | arXiv:2307.00256 |
| `zubrilina-2310.07681.pdf` | N. Zubrilina — *Murmurations* [Z25] (M3 normative) | arXiv:2310.07681 |
| `sawin-sutherland-2504.12295.pdf` | Sawin, Sutherland — *Murmurations for elliptic curves ordered by height* [SS25] (M4 normative) | arXiv:2504.12295 |
| `cowan-2408.12723.pdf` | A. Cowan — *Murmurations and ratios conjectures* [C24] (Q3 context) | arXiv:2408.12723 |
| `sarnak-letter-2726.pdf` | P. Sarnak — letter on murmurations and root numbers [Sar23] (framing) | publications.ias.edu/sarnak/paper/2726 |

Reproducibility pin for these is arXiv ID + version + title-page check, not a
byte-exact sha256 (arXiv re-renders PDFs). The primary M-stage dataset, Cremona's
`ecdata`, is fetched by `oracle/fetch_ecdata.py` into `data/cremona/` against a
sha256-pinned commit — see `data/cremona/README.md`.
