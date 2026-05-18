<script setup lang="ts">
import { computed } from 'vue'

const props = withDefaults(defineProps<{ locale?: 'en' | 'zh' }>(), {
  locale: 'en',
})

const copy = {
  en: {
    title: 'FastQTools architecture overview',
    desc: 'Layered architecture showing CLI, public API, processing core, and evidence outputs.',
    entry: 'ENTRY',
    contract: 'CONTRACT',
    evidence: 'EVIDENCE',
    cliTitle: ['CLI and', 'command layer'],
    cliDetail: ['terminal policy', 'logging and errors'],
    apiTitle: ['Public API', 'integration surface'],
    apiDetail: ['include/fqtools', 'stable contracts'],
    evidenceTitle: ['Benchmark,', 'RFC, reference'],
    evidenceDetail: ['audit trail', 'review surfaces'],
    coreLabel: 'CORE EXECUTION',
    coreTitle: ['source → processing → sink'],
    coreDetail: [
      'bounded batches, zero-copy views, and oneTBB stages',
      'remain the common systems model behind CLI, API, and performance reasoning',
      'so review can move deeper without changing mental frames',
    ],
  },
  zh: {
    title: 'FastQTools 架构总览',
    desc: '展示 CLI、公共 API、处理核心与证据输出之间关系的分层架构图。',
    entry: '入口',
    contract: '契约',
    evidence: '证据',
    cliTitle: ['CLI 与', '命令层'],
    cliDetail: ['终端策略', '日志与错误'],
    apiTitle: ['公共 API', '集成表面'],
    apiDetail: ['include/fqtools', '稳定契约'],
    evidenceTitle: ['Benchmark、', 'RFC 与参考'],
    evidenceDetail: ['审计轨迹', '评审界面'],
    coreLabel: '核心执行',
    coreTitle: ['source → processing → sink'],
    coreDetail: [
      '有界批次、零拷贝视图与 oneTBB 阶段',
      '构成 CLI、API 与性能叙事共享的系统模型',
      '让评审深入时不必切换心智框架',
    ],
  },
} as const

const content = computed(() => copy[props.locale])
</script>

<template>
  <svg
    viewBox="0 0 960 520"
    class="publication-diagram"
    role="img"
    :aria-labelledby="`architecture-overview-title-${props.locale} architecture-overview-desc-${props.locale}`"
  >
    <title :id="`architecture-overview-title-${props.locale}`">{{ content.title }}</title>
    <desc :id="`architecture-overview-desc-${props.locale}`">{{ content.desc }}</desc>

    <rect x="24" y="24" width="912" height="472" rx="28" fill="var(--fq-diagram-bg)" />
    <g class="diagram-grid" stroke="var(--fq-diagram-grid)" stroke-width="1">
      <path d="M120 48 V472" />
      <path d="M300 48 V472" />
      <path d="M480 48 V472" />
      <path d="M660 48 V472" />
      <path d="M840 48 V472" />
      <path d="M48 220 H912" />
      <path d="M48 380 H912" />
    </g>
    <rect x="60" y="72" width="220" height="120" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-stroke)" stroke-width="2" />
    <rect x="370" y="72" width="220" height="120" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-accent)" stroke-width="2.5" />
    <rect x="680" y="72" width="220" height="120" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-signal)" stroke-width="2.5" />
    <rect x="60" y="250" width="840" height="188" rx="28" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-stroke)" stroke-width="2" />

    <path d="M280 132 H370" fill="none" stroke="var(--fq-diagram-stroke)" stroke-width="4" stroke-linecap="round" />
    <path d="M590 132 H680" fill="none" stroke="var(--fq-diagram-accent)" stroke-width="4" stroke-linecap="round" />

    <text x="92" y="110" fill="var(--fq-diagram-muted)" font-size="16" font-weight="700">
      <tspan x="92" dy="0">{{ content.entry }}</tspan>
    </text>
    <text x="402" y="110" fill="var(--fq-diagram-muted)" font-size="16" font-weight="700">
      <tspan x="402" dy="0">{{ content.contract }}</tspan>
    </text>
    <text x="712" y="110" fill="var(--fq-diagram-muted)" font-size="16" font-weight="700">
      <tspan x="712" dy="0">{{ content.evidence }}</tspan>
    </text>

    <text x="92" y="144" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.cliTitle" :key="`cli-title-${line}`" x="92" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="92" y="204" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.cliDetail" :key="`cli-detail-${line}`" x="92" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>

    <text x="402" y="144" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.apiTitle" :key="`api-title-${line}`" x="402" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="402" y="204" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.apiDetail" :key="`api-detail-${line}`" x="402" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>

    <text x="712" y="144" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.evidenceTitle" :key="`evidence-title-${line}`" x="712" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="712" y="204" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.evidenceDetail" :key="`evidence-detail-${line}`" x="712" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>

    <text x="92" y="286" fill="var(--fq-diagram-muted)" font-size="16" font-weight="700">
      <tspan x="92" dy="0">{{ content.coreLabel }}</tspan>
    </text>
    <text x="92" y="330" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.coreTitle" :key="`core-title-${line}`" x="92" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="92" y="376" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.coreDetail" :key="`core-detail-${line}`" x="92" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>
  </svg>
</template>
