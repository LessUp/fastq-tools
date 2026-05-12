---
layout: home
hero:
  name: FastQTools
  text: ' '
  actions:
    - theme: brand
      text: 简体中文
      link: /zh/
    - theme: alt
      text: English
      link: /en/
---

<script setup>
import { onMounted } from 'vue'

onMounted(() => {
  const lang = navigator.language || navigator.userLanguage
  const base = import.meta.env.BASE_URL
  const target = lang.startsWith('zh') ? 'zh/' : 'en/'
  window.location.replace(base + target)
})
</script>
