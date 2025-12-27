@[toc](文章目录)
# 界面总览
大家都有自己下载影像的办法，如果你有比我方便的，也可以打开python源码看看python 如何调用gee的api ，或者学习怎么用C++ 代码深度调用 python 代码（文后有源码贡献）

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/c65a701c92dd44479a7fd9e3fd876660.png)

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/7ac5b1db7ae2473eaa027e06b550c188.png)

# 支持下载的数据

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/0ace6314e5064572982daa56680c586e.png)

```python
    # 影像集合 (ImageCollection) 类
    collections = {
        1: 'COPERNICUS/S2_SR_HARMONIZED',
        2: 'LANDSAT/LC09/C02/T1_L2',
        3: 'LANDSAT/LC08/C02/T1_L2',
        4: 'LANDSAT/LT05/C02/T1_L2',
        5: 'LANDSAT/LE07/C02/T1_L2',
        6: 'MODIS/061/MOD13Q1',
        10: 'COPERNICUS/S1_GRD'
    }
    
    # 单张影像 (Image) 类
    single_images = {
        7: 'USGS/SRTMGL1_003',
        8: 'NASA/NASADEM_HGT/001',
        9: 'JAXA/ALOS/AW3D30/V2_2',
        11: 'ESA/WorldCover/v100',
        12: 'projects/sat-io/open-datasets/landcover/ESRI_Global-Land-Cover-10m/2020'
    }
```
# 项目ID获取方法
需要谷歌账户和外网环境
```
https://code.earthengine.google.com/
```

![](https://i-blog.csdnimg.cn/direct/7c8f196bc333474bad20490109c037a7.png)

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/0e74bc181da940b48db2dd8d40c41fbf.png)

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/d18cfdc781af4fe691c3a9b51512b5af.png)

# 软件使用方法
## 第一步
在 gee 项目 id 输入 你的 id（`此处使用我的没有，在点击下载前会跳转到浏览器 经过你的谷歌账户授权，这里的 id 其实没有什么多大意义`）

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/74a2ec93ca7f4cd4b87da7212c857cd0.png)

## 第二步
选择你要下载的卫星影像，空间分辨率会自动改变，如果下载很慢可以把空间分辨率简单（`对应的数值升高，比如我这里就把 10 改为 20 就是原来分辨率的一半`）

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/1b134d1303ca45f192b174ed91729bf7.png)

`序号`如下图

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/6d9a5adf6e994b8fa98de0762d1df517.png)

时间可以自己选择，注意以下类型的遥感数据是单张影像，时间其实并没有用处。

```python
    # 单张影像 (Image) 类
    single_images = {
        7: 'USGS/SRTMGL1_003',
        8: 'NASA/NASADEM_HGT/001',
        9: 'JAXA/ALOS/AW3D30/V2_2',
        11: 'ESA/WorldCover/v100',
        12: 'projects/sat-io/open-datasets/landcover/ESRI_Global-Land-Cover-10m/2020'
    }
 ```
 同时除了 以下序号需要云最大值，其余数据设置了也没用
 
 ![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/4c22b61620ec44b1843c0a65aa2b8b93.png)

 ```python
 support_cloud_filter = [1, 2, 3, 4, 5]
 ```
 ## 开始下载
 点击此处会依次有`两个文件对话框`
 
 ![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/8f4f72d0920b477293df3e6def457a72.png)
 
### 第一个
选择矢量数据

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/517667bf131348aaae656d9873a4c7ca.png)

### 第二个
选择下载到什么地方

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/164d8823fa8e44c28405787978e87287.png)

点击上图的 “ 保存 ” 后，就开始从云端下载数据了( 下载速度随着网速和下载量不同可能有快有慢 )

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/af4f84c4a2cc4e2499cc4016e290ac54.png)

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/78bceffa422b4bf4ab6dd1d7dfe3212d.png)

# 注意事项
下载的数据是时间范围内 所有满足条件的影像 融合而成的中值影像，融合过程为Google earth engine 云平台进行


 
