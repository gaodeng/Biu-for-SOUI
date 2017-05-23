<template>
  <div id="app" style="text-align:center;height:100%;" class="">

    <Hello ></Hello>

    <div class="download_folder" @click="openDownloadFolder"></div>

  </div>
</template>

<script>
import Hello from './components/Hello.vue'

export default {
  name: 'app',
  components: {
    Hello
  },
  methods: {
    openDownloadFolder: function openDownloadFolder() {

       appshell.app.showOSFolder("c:/")

    }
  }
}







</script>
<style scoped>
.download_folder{
  position: absolute;
  right: 20px;
  bottom:20px;
  background-color: rgba(255,255,255,.96);
  width:40px;
  height:40px;
  box-shadow:2px 3px 6px rgba(0,0,0,.08),0px 0px 1px rgba(0,0,0,.1) ;
  border-radius: 100%;
  background-image:url(./assets/folder.png);
  background-size: 40px 22px;
  background-repeat: no-repeat;
  background-position: center;
  transition: all .4s;
}

.download_folder:hover{
  transform: translate(0px,-4px);
}

.download_folder:active{

  transform: translate(1px,1px);
}

@media only screen and (-webkit-device-pixel-ratio: 2) {
  .download_folder{
    background-image:url('./assets/folder@200.png');

  }
}

@media only screen and (-webkit-device-pixel-ratio: 1.75) {
  .download_folder{
    background-image:url('./assets/folder@175.png');

  }
}

@media only screen and (-webkit-device-pixel-ratio: 1.5) {
  .download_folder{
    background-image:url('./assets/folder@150.png');

  }
}

@media only screen and (-webkit-device-pixel-ratio: 1.25) {
  .download_folder{
    background-image:url('./assets/folder@125.png');

  }
}




</style>


<style>



#app {

  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;

}

html,body {
    background-color: #f6f9fc;
    margin: 0;
    padding: 0;
    color:#4d5256;
    font:14px/22px "微软雅黑", "Microsoft YaHei","Lucida Sans Unicode", Helvetica, Arial, Verdana, sans-serif;
    box-sizing:border-box;
    height: 100%;
    -webkit-user-select: none;


}
*{
  cursor: default;
}

.text-select {
  -webkit-user-select: text;
  cursor: text;
}


::-webkit-scrollbar-track
{

  border-radius: 10px;
}

::-webkit-scrollbar
{
  width: 8px;
    background-color: transparent;
}

::-webkit-scrollbar-thumb
{
  border-radius: 10px;
  background-color: RGBA(0,0,0,0.1);
}


</style>
