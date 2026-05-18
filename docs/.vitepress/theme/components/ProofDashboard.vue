<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type Ledger = {
  title: string
  value: string
  detail: string
  linkId: SharedLinkId
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'Proof dashboard',
    title: 'Anchor the headline claims to concrete review surfaces.',
    summary:
      'This section compresses the homepage argument into the three questions senior reviewers usually ask first: what boundary is maintained, what execution policy explains the speed, and where does the project publish the evidence trail.',
    ledgers: [
      {
        title: 'Product boundary',
        value: 'FASTQ QC core',
        detail: 'Statistics, filtering, trimming, and bounded gzip-aware I/O remain the maintained center of gravity.',
        linkId: 'whitepaperNav',
      },
      {
        title: 'Execution policy',
        value: 'oneTBB + string_view',
        detail: 'Parallel stages and zero-copy record views are described as design contracts, not as incidental optimizations.',
        linkId: 'architecture',
      },
      {
        title: 'Evidence posture',
        value: 'Benchmarks + RFCs',
        detail: 'Benchmark report, reproduction guide, and research appendix keep performance claims reviewable.',
        linkId: 'performance',
      },
    ] satisfies Ledger[],
  },
  zh: {
    eyebrow: '证据仪表板',
    title: '把首页 headline 主张压缩成可直接评审的证据表面。',
    summary:
      '这一节把首页论证收束成高级评审最先会问的三件事：维护中的边界是什么、速度背后的执行政策是什么、以及项目把证据链放在哪里公开。',
    ledgers: [
      {
        title: '产品边界',
        value: 'FASTQ QC 内核',
        detail: '统计、过滤、修剪与有界 gzip I/O 仍然是当前维护中的重心。',
        linkId: 'whitepaperNav',
      },
      {
        title: '执行政策',
        value: 'oneTBB + string_view',
        detail: '并行阶段与零拷贝记录视图被写成设计契约，而不是偶然的优化。',
        linkId: 'architecture',
      },
      {
        title: '证据姿态',
        value: 'Benchmark + RFC',
        detail: 'benchmark 报告、复现实验指南与研究附录共同支撑性能主张。',
        linkId: 'performance',
      },
    ] satisfies Ledger[],
  },
} as const

const content = computed(() => copy[props.locale])
const resolve = (linkId: SharedLinkId) => withBase(getSharedLinkHref(props.locale, linkId))
</script>

<template>
  <section class="home-whitepaper home-section">
    <div class="section-header">
      <p class="section-kicker">{{ content.eyebrow }}</p>
      <h2 class="section-heading">{{ content.title }}</h2>
      <p class="section-lead">{{ content.summary }}</p>
    </div>

    <div class="proof-dashboard-grid">
      <article v-for="item in content.ledgers" :key="item.title" class="paper-card proof-card">
        <p class="evidence-label">{{ item.title }}</p>
        <h3 class="evidence-value">{{ item.value }}</h3>
        <p>{{ item.detail }}</p>
        <a class="paper-link tertiary evidence-link" :href="resolve(item.linkId)">
          <span>{{ props.locale === 'en' ? 'Open review surface' : '打开对应评审面' }}</span>
          <ArrowRight :size="16" />
        </a>
      </article>
    </div>
  </section>
</template>
