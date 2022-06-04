#include<iostream>
#include<fstream>
#include <string>
#include <sstream>
#include <iomanip>  
#include <vector>

using namespace std;

const double  x_pi = 3.14159265358979324 * 3000.0 / 180.0;
const double   pi = 3.1415926535897932384626;  // π
const double    a = 6378245.0;  // 长半轴
const double   ee = 0.00669342162296594323;  // 扁率

double _transformlat(double lng,double lat)
{
    double ret = -100.0 + 2.0 * lng + 3.0 * lat + 0.2 * lat * lat + 0.1 * lng * lat + 0.2 * sqrt(fabs(lng));
        ret += (20.0 * sin(6.0 * lng * pi) + 20.0 * sin(2.0 * lng * pi)) * 2.0 / 3.0;            
        ret += (20.0 * sin(lat * pi) + 40.0 * sin(lat / 3.0 * pi)) * 2.0 / 3.0;
        ret += (160.0 * sin(lat / 12.0 * pi) + 320 * sin(lat * pi / 30.0)) * 2.0 / 3.0;
        return ret;
}

double _transformlng(double lng, double lat)
{
    double ret = 300.0 + lng + 2.0 * lat + 0.1 * lng * lng + 0.1 * lng * lat + 0.1 * sqrt(fabs(lng));
    ret += (20.0 * sin(6.0 * lng * pi) + 20.0 * sin(2.0 * lng * pi)) * 2.0 / 3.0;
    ret += (20.0 * sin(lng * pi) + 40.0 * sin(lng / 3.0 * pi)) * 2.0 / 3.0;
    ret += (150.0 *sin(lng / 12.0 * pi) + 300.0 * sin(lng / 30.0 * pi)) * 2.0 / 3.0;
    return ret;

}

double lng_bd09_to_gcj02(double bd_lon,double bd_lat)
{
    /*
        经度转换
        百度坐标系(BD - 09)转火星坐标系(GCJ - 02)
        百度—— > 谷歌、高德
        :param bd_lat : 百度坐标纬度
        :param bd_lon : 百度坐标经度
        :return : 转换后的坐标列表形式
    */
    double x = bd_lon - 0.0065;
    double y = bd_lat - 0.006;
    double z = sqrt(x * x + y * y) - 0.00002 * sin(y * x_pi);
    double theta = atan2(y, x) - 0.000003 * cos(x * x_pi);
    double gg_lng = z * cos(theta);
    double gg_lat = z * sin(theta);
    
    //cout << setprecision(7)<< gg_lng << "\t" << gg_lat << endl;
    /*
    GCJ02(火星坐标系)转GPS84
    :param lng:火星坐标系的经度
    :param lat:火星坐标系纬度
    :return:
    */
    double dlat = _transformlat(gg_lng - 105.0, gg_lat - 35.0);
    double dlng = _transformlng(gg_lng - 105.0, gg_lat - 35.0);
    double radlat = gg_lat / 180.0 * pi;
    double magic = sin(radlat);
    magic = 1 - ee * magic * magic;
    double sqrtmagic = sqrt(magic);
    dlat = (dlat * 180.0) / ((a * (1 - ee)) / (magic * sqrtmagic) * pi);
    dlng = (dlng * 180.0) / (a / sqrtmagic * cos(radlat) * pi);
    double mglat = gg_lat + dlat;
    double mglng = gg_lng + dlng;

    return mglng;
}

double lat_bd09_to_gcj02(double bd_lon, double bd_lat)
{
    /*
        纬度转换
        百度坐标系(BD - 09)转火星坐标系(GCJ - 02)
        百度—— > 谷歌、高德
        :param bd_lat : 百度坐标纬度
        :param bd_lon : 百度坐标经度
        :return : 转换后的坐标列表形式
    */
    double x = bd_lon - 0.0065;
    double y = bd_lat - 0.006;
    double z = sqrt(x * x + y * y) - 0.00002 * sin(y * x_pi);
    double theta = atan2(y, x) - 0.000003 * cos(x * x_pi);
    double gg_lng = z * cos(theta);
    double gg_lat = z * sin(theta);

    //cout << setprecision(7)<< gg_lng << "\t" << gg_lat << endl;
    /*
    GCJ02(火星坐标系)转GPS84
    :param lng:火星坐标系的经度
    :param lat:火星坐标系纬度
    :return:
    */
    double dlat = _transformlat(gg_lng - 105.0, gg_lat - 35.0);
    double dlng = _transformlng(gg_lng - 105.0, gg_lat - 35.0);
    double radlat = gg_lat / 180.0 * pi;
    double magic = sin(radlat);
    magic = 1 - ee * magic * magic;
    double sqrtmagic = sqrt(magic);
    dlat = (dlat * 180.0) / ((a * (1 - ee)) / (magic * sqrtmagic) * pi);
    dlng = (dlng * 180.0) / (a / sqrtmagic * cos(radlat) * pi);
    double mglat = gg_lat + dlat;
    double mglng = gg_lng + dlng;

    return mglat;
}

void test01()
{
    ifstream ifs;
    ofstream ofs;
    string filename = "eat";
    ifs.open(filename+".txt", ios::in);
    if (!ifs.is_open())
    {
        cout << "文件打开失败" << endl;
        return;
    }

    string buf;      
    vector<double> LNG, LAT;
    vector<string>NAME;
    while (getline(ifs, buf))
    {
        //cout << buf << endl;
        stringstream ss;
        double lat, lng;        //经度 lng  纬度  lat
        double wgs_lat, wgs_lng;   
        string temp_name;
        ss << buf;
        ss>>temp_name>>lng>>lat;
        wgs_lng = lng_bd09_to_gcj02(lng, lat);
        wgs_lat = lat_bd09_to_gcj02(lng, lat);
        NAME.push_back(temp_name);
        LNG.push_back(wgs_lng);
        LAT.push_back(wgs_lat);
    }
    ifs.close();

    int len = LNG.size();    
    ofs.open(filename+"wgs84.csv", ios::trunc);
    for (int i=0;i<len;i++)
    {
        ofs << NAME[i]<<","<<setprecision(11)<< LNG[i] << "," << LAT[i] << endl;
    }
    ofs.close();
    cout << "finish" << endl;
}

int main()
{  
    test01();
    return 0;
}