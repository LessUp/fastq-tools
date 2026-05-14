<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { Archive, Bot, BookOpen, Wrench } from 'lucide-vue-next'
import {
  getSharedLinkHref,
  getSharedLinkLabel,
  resourceHubGroups,
  type LocaleKey,
  type ResourceHubItem,
} from '../content/siteNavigation'

const props = defineProps<{ locale: LocaleKey }>()

const copy = {
  en: {
    title: 'Resource hub',
    intro: 'Everything outside the primary homepage narrative stays grouped here: deep reference, collaboration entry points, automation docs, and historical material.',
    groups: {
      reference: { title: 'Deep reference', summary: 'Move directly into guide, API, and developer material when you already know what you need.', linkLabel: 'Open reference hub' },
      collaboration: { title: 'Collaboration', summary: 'Contribution guidance, release notes, and the repository itself for teams planning active adoption.', linkLabel: 'Open resources hub' },
      automation: { title: 'Automation and agent docs', summary: 'Repository-specific issue-tracker and triage guidance for AI-assisted workflows.', linkLabel: 'Open agent docs' },
      history: { title: 'History and migration context', summary: 'Archive pages remain discoverable without claiming to be current operating guidance.', linkLabel: 'Open archive hub' },
    },
  },
  zh: {
    title: '资源中心',
    intro: '凡是不属于首页主叙事的内容，都统一归到这里：深度参考、协作入口、自动化文档与历史资料。',
    groups: {
      reference: { title: '深度参考', summary: '当你已经知道自己要找什么时，可以直接跳到指南、API 与开发资料。', linkLabel: '打开参考导航' },
      collaboration: { title: '协作入口', summary: '适合准备正式采用项目的团队：贡献说明、发布记录与仓库入口都在这里。', linkLabel: '打开资源中心' },
      automation: { title: '自动化与 Agent 文档', summary: '面向 AI 协作流程的 issue tracker 与 triage 说明。', linkLabel: '打开 Agent 文档' },
      history: { title: '历史与迁移背景', summary: '归档内容仍可检索，但不会被当作当前操作指南。', linkLabel: '打开归档中心' },
    },
  },
} as const

const content = computed(() => copy[props.locale])

const groupIcons = {
  reference: BookOpen,
  collaboration: Wrench,
  automation: Bot,
  history: Archive,
} as const

const hrefFor = (item: ResourceHubItem) => item.type === 'external'
  ? item.href
  : withBase(getSharedLinkHref(props.locale, item.id))

const labelFor = (item: ResourceHubItem) => item.type === 'external'
  ? item.labels[props.locale]
  : getSharedLinkLabel(props.locale, item.id)
</script>

<template>
  <section class="resource-hub">
    <h2>{{ content.title }}</h2>
    <p class="intro">{{ content.intro }}</p>
    <div class="resource-grid">
      <article v-for="group in resourceHubGroups" :key="group.id" class="resource-card">
        <component :is="groupIcons[group.id]" :size="18" class="resource-icon" />
        <h3>{{ content.groups[group.id].title }}</h3>
        <p>{{ content.groups[group.id].summary }}</p>
        <ul>
          <li v-for="item in group.items" :key="item.type === 'external' ? item.href : item.id">
            <a :href="hrefFor(item)">{{ labelFor(item) }}</a>
          </li>
        </ul>
        <a class="resource-link" :href="withBase(getSharedLinkHref(props.locale, group.href))">{{ content.groups[group.id].linkLabel }}</a>
      </article>
    </div>
  </section>
</template>
