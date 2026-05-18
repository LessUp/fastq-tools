<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type CanonGroup = {
  title: string
  summary: string
  links: { label: string; linkId: SharedLinkId }[]
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'Research canon',
    title: 'End the homepage with formal sources, comparative context, and maintenance history.',
    summary:
      'Instead of a generic resources wall, the closing section behaves like the appendix of a systems paper: citation trail, comparative reading, and change history are treated as part of the product story.',
    groups: [
      {
        title: 'Citation trail',
        summary: 'Use formal sources and internal RFCs when you need to defend terminology, benchmark framing, or system boundaries.',
        links: [
          { label: 'Bibliography', linkId: 'bibliography' },
          { label: 'Reference hub', linkId: 'referenceNav' },
        ],
      },
      {
        title: 'Comparative reading',
        summary: 'Compare FastQTools with FastQC, fastp, Cutadapt, and seqtk by scope and evidence style instead of hype.',
        links: [
          { label: 'Related projects', linkId: 'relatedProjects' },
          { label: 'Algorithms', linkId: 'algorithms' },
        ],
      },
      {
        title: 'Evolution record',
        summary: 'Keep the design pressure behind memory policy, benchmark policy, and documentation routing close at hand.',
        links: [
          { label: 'Evolution notes', linkId: 'evolutionNotes' },
          { label: 'Archive overview', linkId: 'archiveOverview' },
        ],
      },
    ] satisfies CanonGroup[],
  },
  zh: {
    eyebrow: '研究正典',
    title: '让首页以正式来源、比较语境与维护历史收束。',
    summary:
      '收尾不再是通用资源墙，而是更像系统论文的附录：引用链、比较阅读与变更历史都被视为产品叙事的一部分。',
    groups: [
      {
        title: '引用链',
        summary: '当你需要为术语、benchmark 口径或系统边界做正式辩护时，请回到来源与内部 RFC。',
        links: [
          { label: '书目', linkId: 'bibliography' },
          { label: '参考导航', linkId: 'referenceNav' },
        ],
      },
      {
        title: '比较阅读',
        summary: '把 FastQTools 与 FastQC、fastp、Cutadapt、seqtk 放回范围和证据风格的比较里，而不是流量式 hype。',
        links: [
          { label: '相关项目', linkId: 'relatedProjects' },
          { label: '算法', linkId: 'algorithms' },
        ],
      },
      {
        title: '演进记录',
        summary: '把内存政策、benchmark 政策与文档分层背后的设计压力保持在手边。',
        links: [
          { label: '演进说明', linkId: 'evolutionNotes' },
          { label: '归档概览', linkId: 'archiveOverview' },
        ],
      },
    ] satisfies CanonGroup[],
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

    <div class="research-rail-grid research-canon-grid">
      <article v-for="group in content.groups" :key="group.title" class="paper-card rail-card">
        <h3>{{ group.title }}</h3>
        <p>{{ group.summary }}</p>
        <ul class="paper-list rail-links">
          <li v-for="link in group.links" :key="link.linkId">
            <a class="rail-link" :href="resolve(link.linkId)">
              <span>{{ link.label }}</span>
              <ArrowRight :size="15" />
            </a>
          </li>
        </ul>
      </article>
    </div>
  </section>
</template>
