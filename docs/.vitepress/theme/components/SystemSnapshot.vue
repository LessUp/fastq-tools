<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type SummaryCard = {
  title: string
  detail: string
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'System snapshot',
    title: 'Trace the data path before diving into individual chapters.',
    summary:
      'The new homepage establishes a technical baseline up front: how compressed input becomes record views, where parallel stages absorb work, and which output surfaces carry validation signals back to readers and operators.',
    cards: [
      {
        title: 'Input normalisation',
        detail: 'Compressed FASTQ sources are framed once so downstream stages work on stable record batches instead of ad-hoc buffers.',
      },
      {
        title: 'Execution isolation',
        detail: 'Parallel stages keep ingest, transforms, and reporting decoupled so performance work does not obscure algorithm reasoning.',
      },
      {
        title: 'Evidence closure',
        detail: 'Benchmark, docs, and reference surfaces are shown as part of the system, not as afterthought appendices.',
      },
    ] satisfies SummaryCard[],
    links: [
      { label: 'Open architecture', linkId: 'architecture' },
      { label: 'Inspect workflows', linkId: 'workflows' },
      { label: 'Review benchmark trail', linkId: 'benchmarkReport' },
    ] satisfies { label: string; linkId: SharedLinkId }[],
  },
  zh: {
    eyebrow: '系统快照',
    title: '先看数据路径，再进入章节细读。',
    summary:
      '新版首页先建立技术基线：压缩输入如何变成记录视图，并行阶段如何吸收工作量，以及哪些输出表面把验证信号返回给读者和操作人员。',
    cards: [
      {
        title: '输入归一化',
        detail: '压缩 FASTQ 数据先被统一分帧，下游阶段处理的是稳定批次，而不是临时拼接的缓冲区。',
      },
      {
        title: '执行解耦',
        detail: '并行阶段把读取、变换与报告隔离开，让性能优化不会吞掉算法解释空间。',
      },
      {
        title: '证据闭环',
        detail: 'benchmark、文档与引用被画进系统里，而不是躲在最后的附录里。',
      },
    ] satisfies SummaryCard[],
    links: [
      { label: '打开架构章节', linkId: 'architecture' },
      { label: '查看工作流', linkId: 'workflows' },
      { label: '追踪基准报告', linkId: 'benchmarkReport' },
    ] satisfies { label: string; linkId: SharedLinkId }[],
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

    <div class="snapshot-grid">
      <div class="paper-grid">
        <article v-for="card in content.cards" :key="card.title" class="paper-card">
          <h3>{{ card.title }}</h3>
          <p>{{ card.detail }}</p>
        </article>

        <footer class="paper-grid-footer">
          <div class="section-actions compact">
            <a
              v-for="link in content.links"
              :key="link.linkId"
              class="paper-link tertiary"
              :href="resolve(link.linkId)"
            >
              <span>{{ link.label }}</span>
              <ArrowRight :size="16" />
            </a>
          </div>
        </footer>
      </div>

      <figure class="paper-card diagram-frame-home">
        <SystemArchitectureDiagram :locale="props.locale" />
      </figure>
    </div>
  </section>
</template>
