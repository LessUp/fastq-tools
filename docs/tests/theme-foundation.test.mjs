import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const styleSource = readFileSync(new URL('../.vitepress/theme/style.css', import.meta.url), 'utf8')
const themeIndexSource = readFileSync(new URL('../.vitepress/theme/index.ts', import.meta.url), 'utf8')

test('theme style imports the four visual layers', () => {
  for (const file of ['tokens.css', 'base.css', 'patterns.css', 'diagrams.css']) {
    assert.match(styleSource, new RegExp(`@import './styles/${file}'`))
  }
})

test('theme index registers only the approved whitepaper components', () => {
  for (const name of ['SiteHeroPanel', 'EvidenceStrip', 'PillarGrid', 'KnowledgeMap', 'WorkflowPaths', 'ResourceHub', 'CitationStrip', 'DiagramFrame', 'SectionLandingGrid']) {
    assert.match(themeIndexSource, new RegExp(`component\\('${name}'`))
  }
})
