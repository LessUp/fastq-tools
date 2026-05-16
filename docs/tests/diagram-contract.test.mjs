import test from 'node:test'
import assert from 'node:assert/strict'
import { readFileSync } from 'node:fs'

const diagramCss = readFileSync(new URL('../.vitepress/theme/styles/diagrams.css', import.meta.url), 'utf8')

test('diagram css defines shared light and dark tokens', () => {
  for (const token of ['--fq-diagram-bg', '--fq-diagram-stroke', '--fq-diagram-text']) {
    assert.match(diagramCss, new RegExp(token))
  }
  assert.match(diagramCss, /\.dark[^{]*\{/)
})

test('shared svg assets use theme-friendly colors', () => {
  for (const file of ['architecture-overview.svg', 'execution-model.svg', 'reading-map.svg']) {
    const source = readFileSync(new URL(`../assets/diagrams/${file}`, import.meta.url), 'utf8')
    assert.match(source, /(currentColor|var\(--fq-diagram-)/)
  }
})
