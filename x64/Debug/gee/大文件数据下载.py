import ee
import os
import geemap  # 引入神器
import geopandas as gpd
from shapely.geometry import mapping
import warnings

# 忽略警告
warnings.filterwarnings("ignore")
# 识别要下载的数据
def SatelliteSelection(index):
    if index == 0:
        raise ValueError("请选择合适的遥感数据")
    
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

    if index in collections:
        return ee.ImageCollection(collections[index]), True
    elif index in single_images:
        return ee.Image(single_images[index]), False
    else:
        raise ValueError("未知的索引")
def download_big_image(shp_path, output_path, index, id, Qscale, start_date, end_date, cloud_filter):
    # ================= 配置 =================
    PROJECT_ID = id
    # 代理 (如果有)
    # os.environ['HTTP_PROXY'] = 'http://127.0.0.1:7890'
    # os.environ['HTTPS_PROXY'] = 'http://127.0.0.1:7890'
    # =======================================
    print(f"正在使用 gee 账户: {PROJECT_ID}")
    # 初始化
    try:
        ee.Initialize(project=PROJECT_ID)
    except Exception as e:
        ee.Authenticate()
        ee.Initialize(project=PROJECT_ID)


    print(f"正在读取矢量: {shp_path}")
    
    # 选择卫星
    data_source, is_collection = SatelliteSelection(index)

    # 处理矢量
    gdf = gpd.read_file(shp_path)
    if gdf.crs != "EPSG:4326":
        gdf = gdf.to_crs("EPSG:4326")
    roi_geom = mapping(gdf.geometry.union_all())
    roi = ee.Geometry(roi_geom)

    # 处理影像
    print("正在筛选影像...")
    if is_collection:
        filtered_col = data_source.filterBounds(roi)\
            .filterDate(start_date, end_date)

        support_cloud_filter = [1, 2, 3, 4, 5]
        # 有云可以用来筛选影像
        if index in support_cloud_filter:
            filtered_col = filtered_col.filter(ee.Filter.lt('CLOUDY_PIXEL_PERCENTAGE', cloud_filter))
        # 不需要用云筛选的影像(modis或者雷达)
        else:
            print(f"检测到合成产品或雷达 (Index {index})，跳过云量筛选...")
        # 输出符合添加的影像数量
        try:
            count = filtered_col.size().getInfo()
            print(f">>> 最终筛选出符合条件的影像数量: {count} 张")
        except Exception as e:
            # 网络问题通常在这里暴露
            raise ValueError(f"连接 GEE 失败: {e}")
        if count == 0:
            raise ValueError(f"在该范围内未找到任何影像！(Index: {index})\n如果是 MODIS，请检查时间范围是否太短。")
        # 以下为原废弃提取数据样式,因为部分数据不用 filter 过滤云
        # image = data_source.filterBounds(roi) \
        #     .filterDate('2021-01-01', '2022-09-30') \
        #     .filter(ee.Filter.lt('CLOUDY_PIXEL_PERCENTAGE', 80)) \
        #     .median().clip(roi)
        image = filtered_col.median().clip(roi)
    else:
        # 如果是单张影像（如DEM），直接裁剪，不进行日期筛选
        image = data_source.clip(roi)

    # 强制转换类型解决 dtype 报错
    image = image.toUint16()

    # 【核心】使用 geemap 导出大文件
    # 它可以自动突破 50MB 限制
    print("开始分块下载合并 (这可能需要几分钟)...")
    
    geemap.download_ee_image(
        image,
        filename=output_path,
        region=roi,
        crs='EPSG:4326',
        scale=Qscale,  # ##✅ 这里可以用 10米了！不用改 50米！
        scale_offset=None  # 可选参数
    )
    
    print(f"影像下载完成: {output_path}")

if __name__ == "__main__":
    import argparse
    import sys

    # 创建参数解析器对象
    parser = argparse.ArgumentParser(description="MineGIS Large Image Downloader")
    # required=True 表示这是必需参数，用户必须提供
    parser.add_argument("--shp", required=True, help="输入: 矿区矢量文件路径 (.shp)", type=str)
    parser.add_argument("--out", required=True, help="输出: 结果保存路径 (.tif)", type=str)
    parser.add_argument("--index", required=True, help="输入: 矿区矢量文件索引", type=int)
    parser.add_argument("--id", required=True, help="输入: gee 账户id", type=str)
    parser.add_argument("--scale", required=True, help="输入: 分辨率", type=int)
    parser.add_argument("--start", required=True, help="输入: 日期范围 (格式: 2023-01-01/2023-01-31)", type=str)
    parser.add_argument("--end", required=True, help="输入: 日期范围 (格式: 2023-01-01/2023-01-31)", type=str)
    parser.add_argument("--cloud", required=False, help="输入: 是否筛选云量 (1: 筛选, 0: 不筛选)", type=int, default=1)

    # 解析参数
    args = parser.parse_args()

    print(f"Python 收到指令:")
    print(f"  - 输入 SHP: {args.shp}")
    print(f"  - 输出 TIF: {args.out}")
    print(f"  - 开始时间: {args.start} - 结束时间: {args.end}")


    # 简单的路径检查
    if os.path.exists(args.shp):
        try:
            download_big_image(\
                args.shp, \
                args.out, \
                int(args.index), \
                args.id, int(args.scale), \
                args.start, \
                args.end, \
                int(args.cloud)
            )
        except Exception as e:
            print(f"Python 运行出错: {e}") # c++ 的readAllStandardOutput接收
            sys.stderr.write(f"Python 运行出错: {str(e)}") # C++ 的readAllStandardError接受
            sys.exit(1) # 返回非0状态码给 C++
    else:
        print(f"错误: 找不到输入文件 {args.shp}")
        sys.exit(1)

    # test_shp = "E:/data/haidian/海淀区.shp"
    # out_tif = "E:/Test/haidian_high_res_10m.tif"
    
    # if os.path.exists(test_shp):
    #     download_big_image(test_shp, out_tif)