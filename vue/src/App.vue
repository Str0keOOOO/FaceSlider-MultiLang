<script setup lang="ts">
import FaceSlider from './components/FaceSlider.vue';

const maxUnhappyCount = 5
const animationSpeed = 0.1
const normalFace = { happiness: 0.9, derp: 1, px: 0.5, py: 0.5 }
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