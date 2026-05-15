import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const navSource = readFileSync(new URL('../.vitepress/theme/content/siteNavigation.ts', import.meta.url), 'utf8')
const configSource = readFileSync(new URL('../.vitepress/config.ts', import.meta.url), 'utf8')

test('top navigation exposes the approved section labels', () => {
  for (const label of ['orientation', 'whitepaper', 'academy', 'reference', 'research']) {
    assert.match(navSource, new RegExp(label, 'i'))
  }
})

test('config wires locale sidebars for the new section roots', () => {
  for (const route of ['/zh/orientation/', '/zh/whitepaper/', '/zh/academy/', '/zh/research/', '/en/orientation/', '/en/whitepaper/', '/en/academy/', '/en/research/']) {
    assert.match(configSource, new RegExp(route.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})
