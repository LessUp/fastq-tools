<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight } from 'lucide-vue-next'
import { getSharedLinkHref, type LocaleKey, type SharedLinkId } from '../content/siteNavigation'

type RailGroup = {
  title: string
  summary: string
  links: { label: string; linkId: SharedLinkId }[]
}

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    eyebrow: 'Research rail',
    title: 'Keep the adjacent material within one scroll of the homepage.',
    summary:
      'The new ending section acts like a whitepaper appendix rail: references, collaboration context, historical notes, and benchmark method pages stay reachable without flattening them into another hero.',
    groups: [
      {
        title: 'Reference stack',
        summary: 'Switch from narrative pages into CLI, API, and developer reference only when needed.',
        links: [
          { label: 'Guide overview', linkId: 'guideOverview' },
          { label: 'API overview', linkId: 'apiOverview' },
        ],
      },
      {
        title: 'Research material',
        summary: 'Open resource maps, contribution expectations, and supporting context with the same locale routing.',
        links: [
          { label: 'Resources', linkId: 'resourcesOverview' },
          { label: 'Contributing', linkId: 'contributing' },
        ],
      },
      {
        title: 'Maintenance history',
        summary: 'Archive and release history remain visible for readers checking trajectory and governance.',
        links: [
          { label: 'Changelog', linkId: 'changelog' },
          { label: 'Archive overview', linkId: 'archiveOverview' },
        ],
      },
    ] satisfies RailGroup[],
  },
  zh: {
    eyebrow: '研究轨道',
    title: '把相邻材料控制在首页的一次滚动范围内。',
    summary:
      '新版收尾像白皮书附录轨道：引用、协作语境、历史说明与 benchmark 方法页始终可达，但不会再次被压扁成另一个 hero。',
    groups: [
      {
        title: '参考栈',
        summary: '只有在确实需要时，才从叙事页切换到 CLI、API 与开发参考。',
        links: [
          { label: '指南概览', linkId: 'guideOverview' },
          { label: 'API 概览', linkId: 'apiOverview' },
        ],
      },
      {
        title: '研究材料',
        summary: '资源地图、贡献要求与补充背景通过同一套 locale 路由暴露出来。',
        links: [
          { label: '资源导航', linkId: 'resourcesOverview' },
          { label: '参与贡献', linkId: 'contributing' },
        ],
      },
      {
        title: '维护历史',
        summary: '归档与发布历史持续可见，方便读者判断项目轨迹与治理方式。',
        links: [
          { label: '变更记录', linkId: 'changelog' },
          { label: '归档概览', linkId: 'archiveOverview' },
        ],
      },
    ] satisfies RailGroup[],
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

    <div class="research-rail-grid">
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
