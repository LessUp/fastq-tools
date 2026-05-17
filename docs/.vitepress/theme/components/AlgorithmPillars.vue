<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type Pillar = {
  title: string
  detail: string
  routeLabel: string
  route: SharedLinkId
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'Algorithm pillars',
    title: 'Three technical commitments define the processing core.',
    summary:
      'Instead of marketing claims, the homepage now exposes the core operating assumptions that appear again across architecture, workflows, API docs, and performance evidence.',
    pillars: [
      {
        title: 'Zero-copy record views',
        detail: 'Record batches stay cheap to traverse so I/O throughput does not collapse under repeated string materialisation.',
        routeLabel: 'Read the I/O story',
        route: 'ioModule',
      },
      {
        title: 'Parallel pipeline boundaries',
        detail: 'oneTBB stages absorb ingestion, transforms, and emission while preserving explicit handoff points for reasoning and testing.',
        routeLabel: 'Inspect workflows',
        route: 'workflows',
      },
      {
        title: 'Statistics as first-class output',
        detail: 'Filtering and trimming are coupled to measurement so operators can justify decisions with counters, summaries, and benchmark traces.',
        routeLabel: 'Open statistics',
        route: 'statistics',
      },
    ] satisfies Pillar[],
  },
  zh: {
    eyebrow: '算法支柱',
    title: '处理核心由三条技术承诺共同定义。',
    summary:
      '首页不再堆砌卖点，而是直接暴露核心运行假设；这些假设会在架构、工作流、API 与性能证据中反复出现。',
    pillars: [
      {
        title: '零拷贝记录视图',
        detail: '批次记录保持轻量遍历，避免 I/O 吞吐在反复字符串实体化中崩塌。',
        routeLabel: '阅读 I/O 说明',
        route: 'ioModule',
      },
      {
        title: '并行流水线边界',
        detail: 'oneTBB 阶段接管读取、变换与发射，同时保留明确的交接点，方便推理与测试。',
        routeLabel: '检查工作流',
        route: 'workflows',
      },
      {
        title: '统计输出是一等公民',
        detail: '过滤与修剪过程被绑定到测量体系，让操作决策能被计数器、摘要和 benchmark 轨迹支撑。',
        routeLabel: '打开统计模块',
        route: 'statistics',
      },
    ] satisfies Pillar[],
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

    <div class="paper-pillars">
      <article v-for="pillar in content.pillars" :key="pillar.title" class="paper-card pillar-card">
        <h3>{{ pillar.title }}</h3>
        <p>{{ pillar.detail }}</p>
        <a class="paper-link tertiary pillar-link" :href="resolve(pillar.route)">
          <span>{{ pillar.routeLabel }}</span>
          <ArrowRight :size="16" />
        </a>
      </article>
    </div>
  </section>
</template>
