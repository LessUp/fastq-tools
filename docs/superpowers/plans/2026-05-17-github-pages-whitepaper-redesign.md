# GitHub Pages Whitepaper Redesign Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Rebuild the FastQTools GitHub Pages site into a bilingual, research-grade whitepaper experience with a new information architecture, theme-aware diagrams, deeper architecture/algorithm content, and a formal citation layer.

**Architecture:** Keep VitePress as the static site engine, but replace the current top-level narrative, homepage component set, navigation data model, and visual system. The implementation centers on a shared content/data layer in `.vitepress/theme/content`, a new homepage + diagram component suite, and new `whitepaper/`, `algorithms/`, and `research/` sections mirrored across `en/` and `zh/`.

**Tech Stack:** VitePress 1.5, Vue 3 single-file components, TypeScript theme modules, Markdown content, Node built-in test runner, Mermaid plugin

---

## File Structure

- Modify: `docs/.vitepress/config.ts` — switch top nav / sidebars to the new Whitepaper–Architecture–Algorithms–Performance–Reference–Research IA.
- Modify: `docs/.vitepress/theme/index.ts` — register the new homepage, citation, and diagram components.
- Modify: `docs/.vitepress/theme/style.css` — replace the current theme tokens and homepage section styling with a stronger whitepaper visual system and dual-theme-safe variables.
- Modify: `docs/.vitepress/theme/content/siteNavigation.ts` — replace the current route registry with the new IA and new shared links.
- Create: `docs/.vitepress/theme/content/siteContent.ts` — shared bilingual copy for homepage sections, chapter intros, metrics, and reading tracks.
- Create: `docs/.vitepress/theme/content/references.ts` — shared bibliography, related-project metadata, and evolution timeline items.
- Create: `docs/.vitepress/theme/components/WhitepaperHero.vue`
- Create: `docs/.vitepress/theme/components/SystemSnapshot.vue`
- Create: `docs/.vitepress/theme/components/AlgorithmPillars.vue`
- Create: `docs/.vitepress/theme/components/EvidenceOverview.vue`
- Create: `docs/.vitepress/theme/components/ReadingTracks.vue`
- Create: `docs/.vitepress/theme/components/ResearchRail.vue`
- Create: `docs/.vitepress/theme/components/SystemArchitectureDiagram.vue`
- Create: `docs/.vitepress/theme/components/ReferenceList.vue`
- Create: `docs/.vitepress/theme/components/ReferenceBadge.vue`
- Modify: `docs/en/index.md` / `docs/zh/index.md` — replace the current homepage component stack with the new whitepaper flow.
- Create: `docs/en/whitepaper/index.md` / `docs/zh/whitepaper/index.md`
- Modify: `docs/en/architecture/index.md` / `docs/zh/architecture/index.md`
- Create: `docs/en/algorithms/index.md` / `docs/zh/algorithms/index.md`
- Modify: `docs/en/performance/index.md` / `docs/zh/performance/index.md`
- Modify: `docs/en/reference/index.md` / `docs/zh/reference/index.md`
- Create: `docs/en/research/index.md` / `docs/zh/research/index.md`
- Create: `docs/en/research/bibliography.md` / `docs/zh/research/bibliography.md`
- Create: `docs/en/research/related-projects.md` / `docs/zh/research/related-projects.md`
- Create: `docs/en/research/evolution-notes.md` / `docs/zh/research/evolution-notes.md`
- Modify: `docs/package.json` — add a docs test script using Node’s built-in test runner.
- Create: `docs/tests/theme-foundation.test.mjs`
- Create: `docs/tests/homepage-contract.test.mjs`
- Create: `docs/tests/research-contract.test.mjs`
- Modify: `docs/assets/images/README.md` — document that the architecture diagram moved to theme-aware components instead of static placeholder SVG.
- Create: `changelog/2026-05-17-github-pages-whitepaper-redesign.md`

### Task 1: Replace the information architecture and theme data foundation

**Files:**
- Modify: `docs/package.json`
- Create: `docs/tests/theme-foundation.test.mjs`
- Modify: `docs/.vitepress/config.ts`
- Modify: `docs/.vitepress/theme/content/siteNavigation.ts`
- Create: `docs/.vitepress/theme/content/siteContent.ts`
- Create: `docs/.vitepress/theme/content/references.ts`
- Modify: `docs/.vitepress/theme/index.ts`
- Modify: `docs/.vitepress/theme/style.css`

- [ ] **Step 1: Write the failing docs foundation tests and expose them through `npm test`**

```json
{
  "scripts": {
    "sync": "node scripts/sync-changelog.mjs",
    "dev": "npm run sync && vitepress dev",
    "test": "node --test tests/*.test.mjs",
    "build": "npm run sync && vitepress build",
    "preview": "vitepress preview"
  }
}
```

```js
// docs/tests/theme-foundation.test.mjs
import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const config = readFileSync(new URL('../.vitepress/config.ts', import.meta.url), 'utf8')
const nav = readFileSync(new URL('../.vitepress/theme/content/siteNavigation.ts', import.meta.url), 'utf8')
const style = readFileSync(new URL('../.vitepress/theme/style.css', import.meta.url), 'utf8')

test('top navigation exposes the new whitepaper IA', () => {
  for (const segment of ['whitepaper', 'architecture', 'algorithms', 'performance', 'reference', 'research']) {
    assert.match(config, new RegExp(segment))
    assert.match(nav, new RegExp(segment))
  }
})

test('theme tokens define both light and dark variables', () => {
  assert.match(style, /:root/)
  assert.match(style, /\.dark/)
  assert.match(style, /--fq-c-canvas/)
  assert.match(style, /--fq-c-brand/)
})
```

- [ ] **Step 2: Run the docs tests to verify the new IA assertions fail against the current site**

Run: `cd docs && npm test`  
Expected: FAIL because `config.ts` and `siteNavigation.ts` still expose the old `why/workflows/knowledge-map/resources` structure.

- [ ] **Step 3: Implement the new shared IA and theme data model**

```ts
// docs/.vitepress/theme/content/siteNavigation.ts
export const topNavLinkIds = [
  'whitepaper',
  'architecture',
  'algorithms',
  'performance',
  'referenceNav',
  'researchNav',
] as const

export const chapterSidebarGroups = [
  { headingKey: 'whitepaperHub', items: ['whitepaperOverview'] },
  { headingKey: 'architectureHub', items: ['architectureOverview'] },
  { headingKey: 'algorithmsHub', items: ['algorithmsOverview'] },
  { headingKey: 'performanceHub', items: ['performanceOverview', 'benchmarkReport'] },
  { headingKey: 'referenceHub', items: ['referenceOverview', 'guideOverview', 'apiOverview', 'devOverview'] },
  { headingKey: 'researchHub', items: ['researchOverview', 'bibliography', 'relatedProjects', 'evolutionNotes'] },
] as const
```

```ts
// docs/.vitepress/theme/content/siteContent.ts
export const homepageSections = {
  en: {
    hero: {
      eyebrow: 'FastQTools Research Docs',
      title: 'System architecture, execution mechanics, and performance evidence for FASTQ QC engineering.',
    },
  },
  zh: {
    hero: {
      eyebrow: 'FastQTools 技术白皮书',
      title: '面向 FASTQ 质控工程的系统架构、执行机制与性能证据站点。',
    },
  },
} as const
```

```ts
// docs/.vitepress/theme/content/references.ts
export const bibliography = [
  {
    id: 'rfc-core-architecture',
    type: 'rfc',
    title: 'RFC-0001: Core Architecture',
    authorsOrOrg: 'FastQTools OpenSpec',
    year: 2026,
    href: 'https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md',
    annotation: 'Defines the layered architecture, zero-copy batches, and oneTBB pipeline posture.',
  },
] as const
```

```ts
// docs/.vitepress/theme/index.ts
import WhitepaperHero from './components/WhitepaperHero.vue'
import SystemSnapshot from './components/SystemSnapshot.vue'
import AlgorithmPillars from './components/AlgorithmPillars.vue'
import EvidenceOverview from './components/EvidenceOverview.vue'
import ReadingTracks from './components/ReadingTracks.vue'
import ResearchRail from './components/ResearchRail.vue'
import SystemArchitectureDiagram from './components/SystemArchitectureDiagram.vue'
import ReferenceList from './components/ReferenceList.vue'
import ReferenceBadge from './components/ReferenceBadge.vue'
```

```css
/* docs/.vitepress/theme/style.css */
:root {
  --fq-c-canvas: #f3f7f9;
  --fq-c-surface: rgba(255, 255, 255, 0.84);
  --fq-c-brand: #0f766e;
  --fq-c-accent: #2563eb;
}

.dark {
  --fq-c-canvas: #071317;
  --fq-c-surface: rgba(11, 23, 27, 0.92);
  --fq-c-brand: #4dd4c6;
  --fq-c-accent: #7ab7ff;
}
```

- [ ] **Step 4: Re-run the foundation tests and the docs build**

Run: `cd docs && npm test && npm run build`  
Expected: PASS, with the test runner finding the new IA identifiers and VitePress building the new navigation tree successfully.

- [ ] **Step 5: Commit**

```bash
git add docs/package.json docs/tests/theme-foundation.test.mjs docs/.vitepress/config.ts docs/.vitepress/theme/content/siteNavigation.ts docs/.vitepress/theme/content/siteContent.ts docs/.vitepress/theme/content/references.ts docs/.vitepress/theme/index.ts docs/.vitepress/theme/style.css
git commit -m "feat(docs): 重建白皮书站点信息架构"
```

### Task 2: Rebuild the homepage and the theme-aware diagram system

**Files:**
- Create: `docs/tests/homepage-contract.test.mjs`
- Create: `docs/.vitepress/theme/components/WhitepaperHero.vue`
- Create: `docs/.vitepress/theme/components/SystemSnapshot.vue`
- Create: `docs/.vitepress/theme/components/AlgorithmPillars.vue`
- Create: `docs/.vitepress/theme/components/EvidenceOverview.vue`
- Create: `docs/.vitepress/theme/components/ReadingTracks.vue`
- Create: `docs/.vitepress/theme/components/ResearchRail.vue`
- Create: `docs/.vitepress/theme/components/SystemArchitectureDiagram.vue`
- Modify: `docs/en/index.md`
- Modify: `docs/zh/index.md`
- Modify: `docs/.vitepress/theme/style.css`

- [ ] **Step 1: Write the failing homepage contract tests**

```js
// docs/tests/homepage-contract.test.mjs
import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const enIndex = readFileSync(new URL('../en/index.md', import.meta.url), 'utf8')
const zhIndex = readFileSync(new URL('../zh/index.md', import.meta.url), 'utf8')
const diagram = readFileSync(new URL('../.vitepress/theme/components/SystemArchitectureDiagram.vue', import.meta.url), 'utf8')

test('homepage uses the new research-grade component stack', () => {
  for (const token of ['WhitepaperHero', 'SystemSnapshot', 'AlgorithmPillars', 'EvidenceOverview', 'ReadingTracks', 'ResearchRail']) {
    assert.match(enIndex, new RegExp(token))
    assert.match(zhIndex, new RegExp(token))
  }
})

test('system diagram is theme-aware instead of using a static dark SVG', () => {
  assert.match(diagram, /var\(--fq-c-/)
  assert.doesNotMatch(diagram, /fill="#1a1a2e"/)
})
```

- [ ] **Step 2: Run the homepage contract tests to confirm they fail before implementation**

Run: `cd docs && node --test tests/homepage-contract.test.mjs`  
Expected: FAIL because the homepage still mounts `SiteHeroPanel`, `EvidenceStrip`, `PillarGrid`, `KnowledgeMap`, `WorkflowPaths`, and `ResourceHub`.

- [ ] **Step 3: Implement the new homepage sections and inline SVG architecture diagram**

```vue
<!-- docs/.vitepress/theme/components/SystemArchitectureDiagram.vue -->
<template>
  <svg viewBox="0 0 960 560" class="system-diagram" role="img" aria-label="FastQTools execution architecture">
    <rect x="48" y="72" width="864" height="416" rx="28" fill="var(--fq-c-surface)" stroke="var(--fq-c-border)" />
    <path d="M200 220H760" stroke="var(--fq-c-brand)" stroke-width="4" stroke-dasharray="8 8" />
    <text x="148" y="182" fill="var(--vp-c-text-1)">CLI / Command Layer</text>
    <text x="148" y="262" fill="var(--vp-c-text-1)">Source → Processing → Sink</text>
    <text x="148" y="342" fill="var(--vp-c-text-1)">FastqBatch / string_view / statistics</text>
  </svg>
</template>
```

```md
<!-- docs/en/index.md -->
<WhitepaperHero locale="en" />
<SystemSnapshot locale="en" />
<AlgorithmPillars locale="en" />
<EvidenceOverview locale="en" />
<ReadingTracks locale="en" />
<ResearchRail locale="en" />
```

```md
<!-- docs/zh/index.md -->
<WhitepaperHero locale="zh" />
<SystemSnapshot locale="zh" />
<AlgorithmPillars locale="zh" />
<EvidenceOverview locale="zh" />
<ReadingTracks locale="zh" />
<ResearchRail locale="zh" />
```

- [ ] **Step 4: Re-run homepage tests and full docs verification**

Run: `cd docs && npm test && npm run build`  
Expected: PASS, with the homepage tests finding the new section components and the site building without unresolved component warnings.

- [ ] **Step 5: Commit**

```bash
git add docs/tests/homepage-contract.test.mjs docs/.vitepress/theme/components/WhitepaperHero.vue docs/.vitepress/theme/components/SystemSnapshot.vue docs/.vitepress/theme/components/AlgorithmPillars.vue docs/.vitepress/theme/components/EvidenceOverview.vue docs/.vitepress/theme/components/ReadingTracks.vue docs/.vitepress/theme/components/ResearchRail.vue docs/.vitepress/theme/components/SystemArchitectureDiagram.vue docs/en/index.md docs/zh/index.md docs/.vitepress/theme/style.css
git commit -m "feat(docs): 重做首页与主题感知图示系统"
```

### Task 3: Add the new Whitepaper, Algorithms, and Research content layers

**Files:**
- Create: `docs/tests/research-contract.test.mjs`
- Create: `docs/.vitepress/theme/components/ReferenceList.vue`
- Create: `docs/.vitepress/theme/components/ReferenceBadge.vue`
- Create: `docs/en/whitepaper/index.md`
- Create: `docs/zh/whitepaper/index.md`
- Modify: `docs/en/architecture/index.md`
- Modify: `docs/zh/architecture/index.md`
- Create: `docs/en/algorithms/index.md`
- Create: `docs/zh/algorithms/index.md`
- Modify: `docs/en/performance/index.md`
- Modify: `docs/zh/performance/index.md`
- Modify: `docs/en/reference/index.md`
- Modify: `docs/zh/reference/index.md`
- Create: `docs/en/research/index.md`
- Create: `docs/zh/research/index.md`
- Create: `docs/en/research/bibliography.md`
- Create: `docs/zh/research/bibliography.md`
- Create: `docs/en/research/related-projects.md`
- Create: `docs/zh/research/related-projects.md`
- Create: `docs/en/research/evolution-notes.md`
- Create: `docs/zh/research/evolution-notes.md`

- [ ] **Step 1: Write the failing content contract tests for the new deep-content layers**

```js
// docs/tests/research-contract.test.mjs
import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const algorithms = readFileSync(new URL('../en/algorithms/index.md', import.meta.url), 'utf8')
const research = readFileSync(new URL('../en/research/index.md', import.meta.url), 'utf8')
const bibliography = readFileSync(new URL('../en/research/bibliography.md', import.meta.url), 'utf8')

test('algorithms page covers filtering, statistics, execution, and memory', () => {
  for (const token of ['Filtering model', 'Statistics model', 'Execution model', 'Memory model']) {
    assert.match(algorithms, new RegExp(token))
  }
})

test('research layer exposes bibliography, related projects, and evolution notes', () => {
  assert.match(research, /bibliography/i)
  assert.match(research, /related projects/i)
  assert.match(research, /evolution/i)
  assert.match(bibliography, /ReferenceList/)
})
```

- [ ] **Step 2: Run the content contract tests and confirm they fail because the new sections do not exist yet**

Run: `cd docs && node --test tests/research-contract.test.mjs`  
Expected: FAIL with missing-file or missing-token errors for `algorithms/` and `research/`.

- [ ] **Step 3: Implement the new bilingual pages and citation components**

```vue
<!-- docs/.vitepress/theme/components/ReferenceList.vue -->
<script setup lang="ts">
import { computed } from 'vue'
import { bibliography } from '../content/references'
const props = defineProps<{ locale: 'en' | 'zh' }>()
const items = computed(() => bibliography)
</script>
```

```md
<!-- docs/en/algorithms/index.md -->
# Algorithms

## Filtering model
Explain how predicate composition and short-circuit filtering keep the data path explicit.

## Statistics model
Explain how per-read and per-position metrics are accumulated.

## Execution model
Explain how oneTBB reserves parallelism for the processing stage.

## Memory model
Explain why `FastqBatch` ownership and `std::string_view` lifetimes matter.
```

```md
<!-- docs/en/research/bibliography.md -->
# Bibliography

<ReferenceList locale="en" />
```

```md
<!-- docs/zh/research/bibliography.md -->
# 参考文献

<ReferenceList locale="zh" />
```

- [ ] **Step 4: Run the research tests plus the full docs build**

Run: `cd docs && npm test && npm run build`  
Expected: PASS, with the new content layers routable from the nav and all Markdown pages resolving their custom components.

- [ ] **Step 5: Commit**

```bash
git add docs/tests/research-contract.test.mjs docs/.vitepress/theme/components/ReferenceList.vue docs/.vitepress/theme/components/ReferenceBadge.vue docs/en/whitepaper/index.md docs/zh/whitepaper/index.md docs/en/architecture/index.md docs/zh/architecture/index.md docs/en/algorithms/index.md docs/zh/algorithms/index.md docs/en/performance/index.md docs/zh/performance/index.md docs/en/reference/index.md docs/zh/reference/index.md docs/en/research/index.md docs/zh/research/index.md docs/en/research/bibliography.md docs/zh/research/bibliography.md docs/en/research/related-projects.md docs/zh/research/related-projects.md docs/en/research/evolution-notes.md docs/zh/research/evolution-notes.md
git commit -m "feat(docs): 扩展白皮书深度内容与研究层"
```

### Task 4: Harden docs delivery, update asset guidance, and record the release

**Files:**
- Modify: `docs/assets/images/README.md`
- Create: `changelog/2026-05-17-github-pages-whitepaper-redesign.md`
- Modify: `docs/package-lock.json` (only if `npm install` is needed after script changes)

- [ ] **Step 1: Write the changelog and asset guidance updates**

```md
<!-- changelog/2026-05-17-github-pages-whitepaper-redesign.md -->
# GitHub Pages 白皮书级重构

- 目标：把文档站重构为面向高级开发者的系统设计与性能白皮书站点。
- 主要改动：新 IA、新首页、新算法层、新研究层、新引用组件、浅深色图示兼容。
- 影响范围：`docs/.vitepress/`、`docs/en/`、`docs/zh/`、`docs/tests/`
- 验证方式：`cd docs && npm test && npm run build`
```

```md
<!-- docs/assets/images/README.md -->
## 当前策略

核心架构图不再依赖固定配色的静态 SVG 作为页面主图，而是改为 `.vitepress/theme/components/` 中的主题感知型内联 SVG 组件。
```

- [ ] **Step 2: Run the final docs verification suite**

Run: `cd docs && npm test && npm run build`  
Expected: PASS, producing a complete VitePress build with no missing pages, unresolved custom components, or route-generation errors.

- [ ] **Step 3: Check the working tree for unintended drift**

Run: `git --no-pager diff --check && git --no-pager status --short`  
Expected: no whitespace errors; only the planned docs/theme/test/changelog files remain modified or newly created.

- [ ] **Step 4: Create the final docs delivery commit**

```bash
git add docs/assets/images/README.md changelog/2026-05-17-github-pages-whitepaper-redesign.md docs/package-lock.json
git commit -m "docs: 完成交付级白皮书站点重构"
```

- [ ] **Step 5: Push the branch**

```bash
git push origin HEAD
```

## Self-Review

- **Spec coverage:** Theme / IA foundation is covered by Task 1; homepage + diagram overhaul by Task 2; Whitepaper / Algorithms / Research content by Task 3; release recording and delivery validation by Task 4.
- **Placeholder scan:** No `TBD`, `TODO`, or “similar to Task N” placeholders remain; each task includes exact file paths, concrete code examples, and explicit commands.
- **Type consistency:** The plan consistently uses `whitepaper`, `algorithms`, `research`, `ReferenceList`, and `SystemArchitectureDiagram` as the canonical names introduced in Tasks 1–3.
