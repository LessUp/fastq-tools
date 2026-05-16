# Diagram Assets

This directory stores shared whitepaper diagrams used across the VitePress site.

## Contract

- Prefer standalone SVG assets for reusable explanatory diagrams.
- Use `currentColor` and/or `var(--fq-diagram-*)` tokens so the assets stay compatible with the shared diagram theme contract.
- Keep diagrams modest in scope: they should reinforce existing page narratives, not replace the page text.
- When embedding in docs pages, prefer the shared `<DiagramFrame>` wrapper so spacing, framing, and captions stay consistent.
