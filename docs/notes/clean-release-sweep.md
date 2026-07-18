# Clean-release sweep — deferred cosmetic/provenance items

Secondary to the science (Derek, 2026-07-11: "a clean, public release is a secondary
goal"). Items to sweep before a public M-stage release / deck extension. None affects
correctness or freshness (which normalizes the stamps); these are provenance-hygiene.

| item | detail | status |
|------|--------|--------|
| M4 snapshot stamp | was `6d50db0-dirty`; re-emitted clean → `558efe8` (commit ce5f974) | **done** |
| M3 snapshot stamp | `viz/data/zubrilina_murmuration.json` carries `601ecfc-dirty` — emitted mid-work, never re-emitted at a clean tree | TODO — chore re-emit at a clean tree |
| all snapshot stamps audit | grep every `viz/data/*.json` `generated_by`; any `-dirty` = emitted from an uncommitted tree → chore re-emit clean (freshness normalizes, so this is cosmetic provenance, not a test failure) | TODO |
| deck M-stage rows | the explainer deck's "18 entries" is the Stage 0–6 count; M-ladder ERRATA rows (#19–#26) + the M-stages are added when the M-stages are presented (RESEARCH/deck deferred) | TODO |
| README scope note | one-line note that the scope outgrew the `primeknots` name deliberately (project-status, rule 7) | TODO |

The chore-re-emit pattern (M2/M3 precedent): after all code is committed and the working
tree is clean, `at emit --stage <s> --out viz/data` so `git describe --dirty` yields a
clean commit hash, then commit the snapshot alone.

## Release metadata (ENDGAME Phase 3/4)

Author identity (Derek, 2026-07-18): ORCID **0009-0001-8101-3908**. Wired into the data-note
authorship block already; the two release-engineering obligations below carry it forward.

| item | detail | status |
|------|--------|--------|
| CITATION.cff (Phase 3) | author `Derek Marshall` with `orcid: https://orcid.org/0009-0001-8101-3908` — use the cff `orcid:` field so Zenodo **pre-fills** it; add title/version/repository/DOI at the release tag | TODO (Phase 3) |
| Zenodo deposit metadata (Phase 4) | deposit-metadata confirmation checklist must **flag** that the author shows ORCID 0009-0001-8101-3908 before minting the DOI | TODO (Phase 4) |
