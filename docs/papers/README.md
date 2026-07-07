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
