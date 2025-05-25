# FaceSlider-MultiLang Vue 组件

这是一个基于 Vue 的滑动变脸器组件，支持自定义配置。

## 示例代码

```html
<script setup lang="ts">
import FaceSlider from './components/FaceSlider.vue';

const maxUnhappyCount = 5;
const animationSpeed = 0.1;
const normalFace = { happiness: 0.9, derp: 1, px: 0.5, py: 0.5 };
const normalUi = {
  btnHappyText: "取消",
  btnUnhappyText: "卸载",
  titleText: "你确定要卸载吗",
  subtitleText: "希望不要卸载"
};
</script>

<template>
  <div class="center-container">
    <FaceSlider :maxUnhappyCount="maxUnhappyCount" :animationSpeed="animationSpeed" :normalFace="normalFace"
      :normalUi="normalUi" />
  </div>
</template>

<style scoped>
.center-container {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100vh;
}
</style>
```

## 属性
| 属性名       | 类型     | 默认值 | 描述                     |
| ------------ | -------- | ------ | ------------------------ |
| `maxUnhappyCount` | `Number` | `5` | 最大拒绝次数 |
| `animationSpeed` | `Number` | `0.1` | 动画时间 |
| `normalFace` | `Object` | `{}` | 正常表情 |
| `normalUi` | `Object` | `{}` | ui文字配置 |

## 贡献
欢迎提交 Issue 或 Pull Request 来改进此组件。
