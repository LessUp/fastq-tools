<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'
import { ArrowRight, BookOpen, Cpu, Wrench } from 'lucide-vue-next'
import { getSharedLinkHref, workflowPathLinks, type LocaleKey } from '../content/siteNavigation'

type PathCard = {
  id: keyof typeof workflowPathLinks
  icon: typeof BookOpen
  command?: string
}

type Copy = {
  title: string
  intro: string
  cards: Record<string, {
    title: string
    audience: string
    summary: string
    steps: string[]
    primaryLabel: string
    secondaryLabel: string
  }>
}

const props = defineProps<{ locale: LocaleKey }>()

const pathCards: PathCard[] = [
  {
    id: 'evaluate',
    icon: BookOpen,
    command: 'FastQTools stat -i sample.fastq.gz -o sample.stat.txt',
  },
  {
    id: 'operate',
    icon: Cpu,
    command: 'FastQTools filter -i input.fastq.gz -o output.fastq.gz --min-quality 20 --min-length 50',
  },
  {
    id: 'contribute',
    icon: Wrench,
  },
]

const copy: Record<LocaleKey, Copy> = {
  en: {
    title: 'Workflow paths',
    intro: 'Academy exposes three recurring reading paths so evaluators, operators, and contributors can start from the page that matches their job.',
    cards: {
      evaluate: {
        title: 'First-run evaluation',
        audience: 'For adopters validating fit',
        summary: 'Install dependencies, build once, and run stat to verify the tool, output shape, and QC reporting model on representative data.',
        steps: ['Review the adoption narrative and performance snapshot.', 'Run a first stat pass on sample or internal FASTQ data.', 'Use the knowledge map to decide whether to continue into operations or reference material.'],
        primaryLabel: 'Start with getting started',
        secondaryLabel: 'Read the adoption story',
      },
      operate: {
        title: 'Routine QC execution',
        audience: 'For analysts and pipeline operators',
        summary: 'Move from configuration and command syntax into repeatable filter workflows with a small, explicit option surface.',
        steps: ['Confirm input and output expectations for stat and filter.', 'Choose thresholds, trimming modes, and runtime controls.', 'Link the workflow page back to performance and benchmark evidence if throughput matters.'],
        primaryLabel: 'Open workflow hub',
        secondaryLabel: 'Check CLI options',
      },
      contribute: {
        title: 'Developer and maintainer path',
        audience: 'For contributors extending the project',
        summary: 'Use the supported scripts/core entry points for build, test, and quality checks, then trace behavior back to the maintained baseline docs.',
        steps: ['Build through the repository entry scripts.', 'Run tests and quality checks before proposing changes.', 'Consult developer docs and baseline specs when touching architecture or APIs.'],
        primaryLabel: 'Open build guide',
        secondaryLabel: 'Open testing guide',
      },
    },
  },
  zh: {
    title: '工作流路径',
    intro: '学院层把常见阅读路径整理成三类，方便评估者、操作者与贡献者从最贴近自己职责的入口开始。',
    cards: {
      evaluate: {
        title: '首次评估路径',
        audience: '面向需要确认适配性的采用者',
        summary: '先安装依赖、完成一次构建，再运行 stat，验证工具是否适合你的数据、报告格式与质控流程。',
        steps: ['先查看采用叙事与性能快照。', '对示例或内部 FASTQ 数据执行第一次 stat。', '再根据知识地图决定继续进入操作路径还是参考资料。'],
        primaryLabel: '从快速开始进入',
        secondaryLabel: '阅读采用叙事',
      },
      operate: {
        title: '常规质控执行路径',
        audience: '面向分析人员与管线操作者',
        summary: '从配置说明与命令语法进入，逐步建立可重复的 filter 流程，同时保持参数面清晰可控。',
        steps: ['确认 stat 与 filter 的输入输出约定。', '选择阈值、修剪模式与运行时控制参数。', '如果吞吐是关键约束，再回到性能与 benchmark 页面核对证据。'],
        primaryLabel: '打开工作流中心',
        secondaryLabel: '查看 CLI 选项',
      },
      contribute: {
        title: '开发者与维护者路径',
        audience: '面向扩展项目的贡献者',
        summary: '通过 scripts/core 入口完成构建、测试与质量检查，再把行为变更回链到维护中的 baseline 文档。',
        steps: ['通过仓库支持的脚本执行构建。', '在提出改动前运行测试与质量检查。', '涉及架构或 API 时，同时查阅开发文档与 baseline 规范。'],
        primaryLabel: '打开构建指南',
        secondaryLabel: '打开测试指南',
      },
    },
  },
}

const content = computed(() => copy[props.locale])
</script>

<template>
  <section class="workflow-paths">
    <h2>{{ content.title }}</h2>
    <p class="intro">{{ content.intro }}</p>
    <div class="path-grid">
      <article v-for="card in pathCards" :key="card.id" class="path-card">
        <component :is="card.icon" :size="18" class="path-icon" />
        <p class="audience">{{ content.cards[card.id].audience }}</p>
        <h3>{{ content.cards[card.id].title }}</h3>
        <p>{{ content.cards[card.id].summary }}</p>
        <code v-if="card.command">{{ card.command }}</code>
        <ol>
          <li v-for="step in content.cards[card.id].steps" :key="step">{{ step }}</li>
        </ol>
        <div class="links">
          <a :href="withBase(getSharedLinkHref(props.locale, workflowPathLinks[card.id].primary))">
            <span>{{ content.cards[card.id].primaryLabel }}</span>
            <ArrowRight :size="16" />
          </a>
          <a :href="withBase(getSharedLinkHref(props.locale, workflowPathLinks[card.id].secondary))">{{ content.cards[card.id].secondaryLabel }}</a>
        </div>
      </article>
    </div>
  </section>
</template>
