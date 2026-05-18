<script setup lang="ts">
import { computed } from 'vue'

const props = withDefaults(defineProps<{ locale?: 'en' | 'zh' }>(), {
  locale: 'en',
})

const copy = {
  en: {
    title: 'FastQTools execution model',
    desc: 'Staged execution path with ingest, processing, sink, and benchmark interpretation surfaces.',
    ingest: ['Ingest', 'and framing'],
    ingestDetail: ['gzip decoding', 'bounded FastqBatch reuse'],
    processing: ['Parallel processing', 'and statistics path'],
    processingDetail: [
      'predicates and mutators',
      'summary metrics and per-position work',
      'oneTBB pipeline orchestration',
      'view lifetime remains explicit',
    ],
    sink: ['Sink and', 'reporting'],
    sinkDetail: ['ordered output', 'summary emission'],
    footer: ['Benchmark interpretation belongs to the whole path, not to one isolated stage.'],
  },
  zh: {
    title: 'FastQTools 执行模型',
    desc: '展示输入、处理、输出与 benchmark 解释界面的分阶段执行路径。',
    ingest: ['输入摄取', '与分帧'],
    ingestDetail: ['gzip 解码', '有界 FastqBatch 复用'],
    processing: ['并行处理', '与统计路径'],
    processingDetail: [
      'predicate 与 mutator',
      '汇总指标与逐位点计算',
      'oneTBB 流水线编排',
      '视图生命周期保持显式',
    ],
    sink: ['输出汇聚', '与报告'],
    sinkDetail: ['有序输出', '摘要生成'],
    footer: ['Benchmark 解释属于整条路径，而不是某一个孤立阶段。'],
  },
} as const

const content = computed(() => copy[props.locale])
</script>

<template>
  <svg
    viewBox="0 0 960 520"
    class="publication-diagram"
    role="img"
    :aria-labelledby="`execution-model-title-${props.locale} execution-model-desc-${props.locale}`"
  >
    <title :id="`execution-model-title-${props.locale}`">{{ content.title }}</title>
    <desc :id="`execution-model-desc-${props.locale}`">{{ content.desc }}</desc>

    <rect x="24" y="24" width="912" height="472" rx="28" fill="var(--fq-diagram-bg)" />
    <rect x="60" y="142" width="220" height="160" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-stroke)" stroke-width="2" />
    <rect x="370" y="102" width="220" height="240" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-accent)" stroke-width="2.5" />
    <rect x="680" y="142" width="220" height="160" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-signal)" stroke-width="2.5" />
    <rect x="304" y="390" width="352" height="70" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-stroke)" stroke-width="2" />

    <path d="M280 222 H370" fill="none" stroke="var(--fq-diagram-stroke)" stroke-width="4" stroke-linecap="round" />
    <path d="M590 222 H680" fill="none" stroke="var(--fq-diagram-accent)" stroke-width="4" stroke-linecap="round" />
    <path d="M480 342 V390" fill="none" stroke="var(--fq-diagram-stroke)" stroke-width="4" stroke-linecap="round" />

    <text x="92" y="174" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.ingest" :key="`ingest-${line}`" x="92" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="92" y="236" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.ingestDetail" :key="`ingest-detail-${line}`" x="92" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>

    <text x="402" y="142" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.processing" :key="`processing-${line}`" x="402" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="402" y="214" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.processingDetail" :key="`processing-detail-${line}`" x="402" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>

    <text x="712" y="174" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.sink" :key="`sink-${line}`" x="712" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="712" y="236" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.sinkDetail" :key="`sink-detail-${line}`" x="712" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>

    <text x="336" y="430" fill="var(--fq-diagram-text)" font-size="18" font-weight="600">
      <tspan v-for="(line, index) in content.footer" :key="`footer-${line}`" x="336" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>
  </svg>
</template>
