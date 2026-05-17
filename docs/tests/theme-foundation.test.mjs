import test from 'node:test'
import assert from 'node:assert/strict'
import { existsSync, readFileSync } from 'node:fs'

const packageJson = JSON.parse(readFileSync(new URL('../package.json', import.meta.url), 'utf8'))
const navSource = readFileSync(new URL('../.vitepress/theme/content/siteNavigation.ts', import.meta.url), 'utf8')
const configSource = readFileSync(new URL('../.vitepress/config.ts', import.meta.url), 'utf8')

test('docs package exposes the theme foundation test through npm test', () => {
  assert.match(packageJson.scripts.test, /theme-foundation\.test\.mjs/)
})

test('theme foundation promotes the whitepaper-first IA at the top level', () => {
  const topLevelBlock = navSource.match(/topNavLinkIds\s*=\s*\[(.*?)\]\s*as const/s)?.[1] ?? ''

  for (const id of ['whitepaper', 'architecture', 'algorithms', 'performance', 'referenceNav', 'researchNav']) {
    assert.match(topLevelBlock, new RegExp(`['"]${id}['"]`), `missing ${id} in top navigation`)
  }

  assert.doesNotMatch(topLevelBlock, /orientationNav/)
  assert.doesNotMatch(topLevelBlock, /academyNav/)
})

test('theme foundation ships dedicated shared content and reference data modules', () => {
  for (const relativePath of [
    '../.vitepress/theme/content/siteContent.ts',
    '../.vitepress/theme/content/references.ts',
  ]) {
    assert.equal(existsSync(new URL(relativePath, import.meta.url)), true, `${relativePath} should exist`)
  }
})

test('config active matches pivot around the new IA concepts', () => {
  for (const token of ['whitepaper:', 'architecture:', 'algorithms:', 'performance:', 'referenceNav:', 'researchNav:']) {
    assert.match(configSource, new RegExp(token))
  }

  assert.doesNotMatch(configSource, /orientationNav:/)
  assert.doesNotMatch(configSource, /academyNav:/)
})
