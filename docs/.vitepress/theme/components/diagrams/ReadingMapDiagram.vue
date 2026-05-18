<script setup lang="ts">
import { computed } from 'vue'

const props = withDefaults(defineProps<{ locale?: 'en' | 'zh' }>(), {
  locale: 'en',
})

const copy = {
  en: {
    title: 'FastQTools reading map',
    desc: 'Reading map across whitepaper, architecture, algorithms, performance, reference, and research.',
    whitepaper: ['Whitepaper'],
    whitepaperDetail: ['scope and review frame'],
    architecture: ['Architecture'],
    architectureDetail: ['boundaries and control surfaces'],
    performance: ['Performance'],
    performanceDetail: ['benchmark envelope'],
    algorithms: ['Algorithms'],
    algorithmsDetail: ['filtering, stats, memory discipline'],
    reference: ['Reference'],
    referenceDetail: ['commands, APIs, contributor surfaces'],
    research: ['Research'],
    researchDetail: ['bibliography, comparison, evolution'],
  },
  zh: {
    title: 'FastQTools 阅读地图',
    desc: '展示白皮书、架构、算法、性能、参考与研究层之间的阅读路线。',
    whitepaper: ['白皮书'],
    whitepaperDetail: ['范围与评审框架'],
    architecture: ['架构'],
    architectureDetail: ['边界与控制界面'],
    performance: ['性能'],
    performanceDetail: ['benchmark 包络'],
    algorithms: ['算法'],
    algorithmsDetail: ['过滤、统计与内存约束'],
    reference: ['参考'],
    referenceDetail: ['命令、API 与贡献界面'],
    research: ['研究'],
    researchDetail: ['书目、比较与演进'],
  },
} as const

const content = computed(() => copy[props.locale])
</script>

<template>
  <svg
    viewBox="0 0 960 520"
    class="publication-diagram"
    role="img"
    :aria-labelledby="`reading-map-title-${props.locale} reading-map-desc-${props.locale}`"
  >
    <title :id="`reading-map-title-${props.locale}`">{{ content.title }}</title>
    <desc :id="`reading-map-desc-${props.locale}`">{{ content.desc }}</desc>

    <rect x="24" y="24" width="912" height="472" rx="28" fill="var(--fq-diagram-bg)" />
    <g class="diagram-grid" stroke="var(--fq-diagram-grid)" stroke-width="1">
      <path d="M186 54 V430" />
      <path d="M480 54 V430" />
      <path d="M774 54 V430" />
      <path d="M56 134 H904" />
      <path d="M56 298 H904" />
      <path d="M56 430 H904" />
    </g>
    <rect x="76" y="84" width="220" height="100" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-accent)" stroke-width="2.5" />
    <rect x="370" y="84" width="220" height="100" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-stroke)" stroke-width="2" />
    <rect x="664" y="84" width="220" height="100" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-signal)" stroke-width="2.5" />
    <rect x="76" y="248" width="220" height="100" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-stroke)" stroke-width="2" />
    <rect x="370" y="248" width="220" height="100" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-accent)" stroke-width="2.5" />
    <rect x="664" y="248" width="220" height="100" rx="24" fill="var(--fq-diagram-layer)" stroke="var(--fq-diagram-signal)" stroke-width="2.5" />

    <path d="M296 134 H370" fill="none" stroke="var(--fq-diagram-accent)" stroke-width="4" stroke-linecap="round" />
    <path d="M590 134 H664" fill="none" stroke="var(--fq-diagram-signal)" stroke-width="4" stroke-linecap="round" />
    <path d="M186 184 V248" fill="none" stroke="var(--fq-diagram-stroke)" stroke-width="4" stroke-linecap="round" />
    <path d="M480 184 V248" fill="none" stroke="var(--fq-diagram-accent)" stroke-width="4" stroke-linecap="round" />
    <path d="M774 184 V248" fill="none" stroke="var(--fq-diagram-signal)" stroke-width="4" stroke-linecap="round" />

    <text x="108" y="122" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.whitepaper" :key="`whitepaper-${line}`" x="108" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="108" y="152" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.whitepaperDetail" :key="`whitepaper-detail-${line}`" x="108" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>

    <text x="402" y="122" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.architecture" :key="`architecture-${line}`" x="402" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="402" y="152" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.architectureDetail" :key="`architecture-detail-${line}`" x="402" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>

    <text x="696" y="122" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.performance" :key="`performance-${line}`" x="696" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="696" y="152" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.performanceDetail" :key="`performance-detail-${line}`" x="696" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>

    <text x="108" y="286" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.algorithms" :key="`algorithms-${line}`" x="108" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="108" y="316" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.algorithmsDetail" :key="`algorithms-detail-${line}`" x="108" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>

    <text x="402" y="286" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.reference" :key="`reference-${line}`" x="402" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="402" y="316" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.referenceDetail" :key="`reference-detail-${line}`" x="402" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>

    <text x="696" y="286" fill="var(--fq-diagram-text)" font-size="22" font-weight="700">
      <tspan v-for="(line, index) in content.research" :key="`research-${line}`" x="696" :dy="index === 0 ? 0 : 28">{{ line }}</tspan>
    </text>
    <text x="696" y="316" fill="var(--fq-diagram-text)" font-size="18">
      <tspan v-for="(line, index) in content.researchDetail" :key="`research-detail-${line}`" x="696" :dy="index === 0 ? 0 : 24">{{ line }}</tspan>
    </text>
  </svg>
</template>
