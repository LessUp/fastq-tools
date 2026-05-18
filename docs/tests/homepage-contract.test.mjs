import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const readUtf8 = (relativePath) => readFileSync(new URL(relativePath, import.meta.url), 'utf8')

const zhHome = readUtf8('../zh/index.md')
const enHome = readUtf8('../en/index.md')
const themeSource = readUtf8('../.vitepress/theme/index.ts')
const snapshotSource = readUtf8('../.vitepress/theme/components/SystemSnapshot.vue')
const themeStyles = readUtf8('../.vitepress/theme/style.css')

const homepageContracts = [
  {
    locale: 'zh',
    source: zhHome,
    expectedMarkup: [
      '<PublicationHero locale="zh" />',
      '<ProofDashboard locale="zh" />',
      '<SystemAtlas locale="zh" />',
      '<EvidenceDeck locale="zh" />',
      '<ReadingLanes locale="zh" />',
      '<ResearchCanon locale="zh" />',
    ],
  },
  {
    locale: 'en',
    source: enHome,
    expectedMarkup: [
      '<PublicationHero locale="en" />',
      '<ProofDashboard locale="en" />',
      '<SystemAtlas locale="en" />',
      '<EvidenceDeck locale="en" />',
      '<ReadingLanes locale="en" />',
      '<ResearchCanon locale="en" />',
    ],
  },
]

test('homepages use the new research-grade component stack', () => {
  for (const { locale, source, expectedMarkup } of homepageContracts) {
    for (const markup of expectedMarkup) {
      assert.match(
        source,
        new RegExp(markup.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')),
        `${locale} homepage should include ${markup}`,
      )
    }
  }
})

test('theme registers the rebuilt homepage components', () => {
  for (const componentName of [
    'PublicationHero',
    'ProofDashboard',
    'SystemAtlas',
    'EvidenceDeck',
    'ReadingLanes',
    'ResearchCanon',
  ]) {
    assert.match(
      themeSource,
      new RegExp(`component\\(['"]${componentName}['"]`),
      `theme should register ${componentName}`,
    )
  }
})

test('system atlas is an inline theme-aware svg with publication-grade tokens', () => {
  assert.doesNotThrow(() => readUtf8('../.vitepress/theme/components/SystemAtlas.vue'))
  const diagramSource = readUtf8('../.vitepress/theme/components/SystemAtlas.vue')

  assert.match(diagramSource, /<svg[\s\S]*viewBox=/)
  assert.match(diagramSource, /--fq-diagram-(?:bg|layer|accent|signal|stroke|text|muted)/)
  assert.doesNotMatch(diagramSource, /#0[0-9a-f]{2,7}/i)
})

test('system snapshot promotes the CTA links into a full-width section footer', () => {
  assert.match(snapshotSource, /<footer class="paper-grid-footer">/)
  assert.match(snapshotSource, /<div class="section-actions compact">/)
  assert.match(themeStyles, /\.paper-grid-footer\s*\{[^}]*grid-column:\s*1\s*\/\s*-1;/s)
})

test('homepage grids size cards with responsive min widths instead of fixed three-column tracks', () => {
  assert.match(
    themeStyles,
    /\.paper-grid,\s*\.paper-pillars,\s*\.evidence-grid,\s*(?:\.proof-dashboard-grid,\s*)?\.track-grid,\s*\.research-rail-grid\s*\{[\s\S]*grid-template-columns:\s*repeat\(auto-fit,\s*minmax\(min\(100%,\s*18rem\),\s*1fr\)\);/s,
  )
  assert.match(
    themeStyles,
    /@media \(max-width:\s*1100px\)\s*\{[\s\S]*\.publication-hero-grid,[\s\S]*\.proof-dashboard-grid,[\s\S]*\.system-atlas-grid,[\s\S]*grid-template-columns:\s*1fr;/s,
  )
})

test('homepage hero uses the specimen-sheet redesign vocabulary and rhythm', () => {
  const heroSource = readUtf8('../.vitepress/theme/components/PublicationHero.vue')

  assert.match(heroSource, /Specimen\s+01|标本\s+01/)
  assert.match(heroSource, /systems review sheet|系统评审样张/)
  assert.match(themeStyles, /\.publication-hero::before\s*\{[\s\S]*linear-gradient\(90deg/s)
  assert.match(themeStyles, /\.publication-proof-grid\s*\{[\s\S]*grid-template-columns:\s*1fr;/s)
})

test('homepage visual system avoids template card monotony with ledger and runway regions', () => {
  assert.match(themeStyles, /\.hero-runway\s*\{[\s\S]*writing-mode:\s*vertical-rl;/s)
  assert.match(themeStyles, /\.evidence-deck\s+\.evidence-grid\s*\{[\s\S]*grid-template-columns:\s*minmax\(0,\s*1\.1fr\)\s+minmax\(18rem,\s*0\.75fr\);/s)
  assert.match(themeStyles, /\.research-canon-grid\s*\{[\s\S]*grid-template-columns:\s*minmax\(18rem,\s*0\.8fr\)\s+minmax\(0,\s*1fr\);/s)
})
