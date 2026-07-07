# docs/schemas/

Versioned JSON Schemas for the files emitted by `src/emit/` and consumed by the
`viz/` viewers (see ARCHITECTURE.md §5). Each schema is added by the stage that
first emits data against it — `linking_matrix.schema.json` with Stage 1,
`borromean.schema.json` with Stage 2, and so on.

Stage 0 emits no visualization data, so this directory is intentionally empty of
schemas today.
