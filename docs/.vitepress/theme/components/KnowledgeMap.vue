<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { BookOpen, Boxes, Cpu, Wrench } from 'lucide-vue-next'
import {
  getSharedLinkHref,
  getSharedLinkLabel,
  knowledgeMapSections,
  type LocaleKey,
} from '../content/siteNavigation'

type Copy = {
  title: string
  intro: string
  sections: Record<string, { title: string; summary: string; linkLabel: string }>
}

const props = defineProps<{ locale: LocaleKey }>()

const copy: Record<LocaleKey, Copy> = {
  en: {
    title: 'Knowledge map',
    intro: 'The homepage routes readers into four stable knowledge domains so evaluation, operations, and implementation material stay easy to find.',
    sections: {
      evaluate: {
        title: 'Evaluate the fit',
        summary: 'Start with positioning, architecture constraints, and performance evidence before you commit to a workflow change.',
        linkLabel: 'Open narrative track',
      },
      operate: {
        title: 'Operate the workflows',
        summary: 'Move from the first stat run to day-to-day filter usage with a small, explicit command surface.',
        linkLabel: 'Open operator track',
      },
      reference: {
        title: 'Reference the interfaces',
        summary: 'Use the reference hub when you need exact command flags, API boundaries, or development entry points.',
        linkLabel: 'Open reference track',
      },
      extend: {
        title: 'Extend and collaborate',
        summary: 'Contribution guides, agent docs, and archive material are kept nearby without crowding the main adoption story.',
        linkLabel: 'Open resource track',
      },
    },
  },
  zh: {
    title: '知识地图',
    intro: '首页把内容整理为四个稳定知识域，方便读者分别处理评估、操作、实现与协作场景。',
    sections: {
      evaluate: {
        title: '先判断是否适配',
        summary: '在调整工作流之前，先查看项目定位、架构约束与性能证据。',
        linkLabel: '打开叙事路径',
      },
      operate: {
        title: '再进入操作工作流',
        summary: '从第一次 stat 运行走到日常 filter 使用，命令面保持小而明确。',
        linkLabel: '打开操作路径',
      },
      reference: {
        title: '需要时查接口细节',
        summary: '当你需要精确参数、API 边界或开发入口时，再进入参考导航。',
        linkLabel: '打开参考路径',
      },
      extend: {
        title: '延伸到协作与维护',
        summary: '贡献说明、Agent 文档与归档内容被保留在附近，但不会挤占主叙事。',
        linkLabel: '打开资源路径',
      },
    },
  },
}

const content = computed(() => copy[props.locale])

const sectionIcons = {
  evaluate: BookOpen,
  operate: Cpu,
  reference: Boxes,
  extend: Wrench,
} as const
</script>

<template>
  <section class="knowledge-map">
    <h2>{{ content.title }}</h2>
    <p class="intro">{{ content.intro }}</p>
    <div class="map-grid">
      <article v-for="section in knowledgeMapSections" :key="section.id" class="map-card">
        <component :is="sectionIcons[section.id]" :size="18" class="map-icon" />
        <h3>{{ content.sections[section.id].title }}</h3>
        <p>{{ content.sections[section.id].summary }}</p>
        <ul>
          <li v-for="item in section.items" :key="item">
            <a :href="withBase(getSharedLinkHref(props.locale, item))">{{ getSharedLinkLabel(props.locale, item) }}</a>
          </li>
        </ul>
        <a class="section-link" :href="withBase(getSharedLinkHref(props.locale, section.href))">{{ content.sections[section.id].linkLabel }}</a>
      </article>
    </div>
  </section>
</template>
