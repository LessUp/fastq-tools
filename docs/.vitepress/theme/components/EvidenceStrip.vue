<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { Cpu, Database, Gauge, ShieldCheck } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey } from '../content/siteNavigation'

type Metric = {
  label: string
  value: string
  note: string
  icon: typeof Gauge
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    title: 'Evidence checkpoint for the reading path',
    intro: 'These benchmark figures are the maintained proof points readers should validate while moving from the whitepaper narrative into research and execution planning.',
    metrics: [
      { label: 'FASTQ read path', value: '1696 MB/s', note: 'Parsing and ingest throughput on the maintained benchmark set.', icon: Gauge },
      { label: 'FASTQ write path', value: '1.76M reads/s', note: 'Output throughput for the release benchmark profile.', icon: Database },
      { label: 'Combined filter pass', value: '1.67M reads/s', note: 'A realistic QC run with multiple predicates enabled.', icon: Cpu },
      { label: 'Full statistics pass', value: '302 MB/s', note: 'End-to-end metrics collection with per-position reporting.', icon: ShieldCheck },
    ] satisfies Metric[],
    footnote: 'Benchmarks are representative, not universal guarantees. Review methodology, benchmark reports, and adjacent research material before making procurement or pipeline commitments.',
    linkLabel: 'Review performance evidence',
    linkId: 'performance',
  },
  zh: {
    title: '阅读路径中的证据检查点',
    intro: '下面这组 benchmark 数字是白皮书与研究层共用的维护证据，帮助读者在进入实施规划前先完成一次快速核对。',
    metrics: [
      { label: 'FASTQ 读取路径', value: '1696 MB/s', note: '维护基准集上的解析与摄取吞吐。', icon: Gauge },
      { label: 'FASTQ 写出路径', value: '1.76M reads/s', note: 'Release 基准配置下的输出吞吐。', icon: Database },
      { label: '组合过滤流程', value: '1.67M reads/s', note: '启用多种谓词后的真实质控流程表现。', icon: Cpu },
      { label: '完整统计流程', value: '302 MB/s', note: '包含逐位置报告的端到端统计速度。', icon: ShieldCheck },
    ] satisfies Metric[],
    footnote: '这些结果是代表性样本，而不是对所有环境的统一保证。做选型或管线承诺前，请继续查看方法学、benchmark 报告与相关研究资料。',
    linkLabel: '查看性能证据',
    linkId: 'performance',
  },
} as const

const content = computed(() => copy[props.locale])
</script>

<template>
  <section class="evidence-strip">
    <div class="section-copy">
      <h2>{{ content.title }}</h2>
      <p>{{ content.intro }}</p>
    </div>

    <div class="metric-grid">
      <article v-for="metric in content.metrics" :key="metric.label" class="metric-card">
        <component :is="metric.icon" :size="18" class="metric-icon" />
        <p class="metric-label">{{ metric.label }}</p>
        <p class="metric-value">{{ metric.value }}</p>
        <p class="metric-note">{{ metric.note }}</p>
      </article>
    </div>

    <footer class="evidence-footer">
      <p>{{ content.footnote }}</p>
      <a :href="withBase(getSharedLinkHref(props.locale, content.linkId))">{{ content.linkLabel }}</a>
    </footer>
  </section>
</template>
