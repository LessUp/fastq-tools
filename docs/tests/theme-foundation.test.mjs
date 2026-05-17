import test from 'node:test'
import assert from 'node:assert/strict'
import { existsSync, readFileSync } from 'node:fs'

const styleSource = readFileSync(new URL('../.vitepress/theme/style.css', import.meta.url), 'utf8')
const themeIndexSource = readFileSync(new URL('../.vitepress/theme/index.ts', import.meta.url), 'utf8')
const stylesDirUrl = new URL('../.vitepress/theme/styles/', import.meta.url)
const tokensSource = readFileSync(new URL('tokens.css', stylesDirUrl), 'utf8')
const diagramsSource = readFileSync(new URL('diagrams.css', stylesDirUrl), 'utf8')
const packageSource = readFileSync(new URL('../package.json', import.meta.url), 'utf8')
const ciSource = readFileSync(new URL('../../.github/workflows/ci-docs.yml', import.meta.url), 'utf8')
const changelogPath = new URL('../../changelog/2026-05-15-docs-whitepaper-rebuild.md', import.meta.url)

test('theme style imports the four visual layers', () => {
  for (const file of ['tokens.css', 'base.css', 'patterns.css', 'diagrams.css']) {
    assert.match(styleSource, new RegExp(`@import './styles/${file}'`))
  }
})

test('imported theme layer files exist on disk', () => {
  for (const file of ['tokens.css', 'base.css', 'patterns.css', 'diagrams.css']) {
    assert.equal(existsSync(new URL(file, stylesDirUrl)), true)
  }
})

test('tokens layer preserves foundational color schemes and theme tokens', () => {
  assert.match(tokensSource, /:root\s*\{[\s\S]*color-scheme:\s*light;/)
  assert.match(tokensSource, /\.dark\s*\{[\s\S]*color-scheme:\s*dark;/)

  for (const tokenName of [
    '--fq-c-canvas',
    '--fq-c-surface',
    '--fq-c-surface-alt',
    '--fq-c-text-main',
    '--fq-c-border',
    '--fq-c-brand',
    '--vp-c-brand-1',
    '--vp-c-bg',
    '--vp-c-text-1',
    '--vp-home-hero-name-color',
  ]) {
    assert.match(tokensSource, new RegExp(`${tokenName}:`))
  }
})

test('diagrams layer keeps diagram and mermaid styling hooks', () => {
  for (const selector of ['.diagram-frame', '.mermaid']) {
    assert.match(diagramsSource, new RegExp(selector.replace(/\./g, '\\.')))
  }
})

test('theme index registers the approved whitepaper components', () => {
  for (const name of ['SiteHeroPanel', 'EvidenceStrip', 'PillarGrid', 'KnowledgeMap', 'WorkflowPaths', 'ResourceHub', 'CitationStrip', 'DiagramFrame', 'SectionLandingGrid']) {
    assert.match(themeIndexSource, new RegExp(`component\\('${name}'`))
  }
})

test('docs package exposes a docs source verification command', () => {
  assert.match(packageSource, /"test":\s*"node --test [^"]*theme-foundation\.test\.mjs/)
  assert.match(packageSource, /en-mirror-matrix\.test\.mjs/)
})

test('docs ci runs source tests before vitepress build', () => {
  assert.match(ciSource, /name:\s*Run docs source tests[\s\S]*run:\s*npm test/)
  assert.match(ciSource, /name:\s*Build docs[\s\S]*run:\s*npm run build/)
})

test('whitepaper rebuild changelog record exists', () => {
  assert.equal(existsSync(changelogPath), true)
  const changelogSource = readFileSync(changelogPath, 'utf8')
  assert.match(changelogSource, /Docs whitepaper rebuild|文档白皮书重构/)
  assert.match(changelogSource, /npm test/)
  assert.match(changelogSource, /npm run build/)
})
