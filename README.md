# 重要通知
这里的演示代码是v1.0.2之前的，以后将陆续更新，新文档参见：
[doc](https://github.com/webcpp/webcppd/blob/master/doc/webcppd-handbook.pdf)
# webcppd-example
webcppd动态服务器插件式开发范例

# 依赖(Centos)
* epel-release：`yum -y install epel-release`
* Poco： `yum -y install poco-devel`
* [webcppd](https://github.com/webcpp/webcppd)
* opencv：`yum -y install opencv-devel`
* qrencode: `yum -y install qrencode-devel`
* ubuntu 用户请自行寻找对应依赖

# 使用方法

* 直接
`
make
`


* 配置webcppd
> * http.libHandlerDir 
> * http.tplDirectory


分别指向webcppd-examaple的build和tpl目录.

* 运行webcppd服务器.

* 最后访问
> `http://localhost:8888/helloworld/index`
.

# 演示网站
> [http://demo.webcpp.net](http://demo.webcpp.net)

> ## 全部演示列表

* home
* static
* cookie
* login
* logout
* gzip
* get
* post
* upload
* ajax
* captcha
* cropper,blur,gaussion blur, median blur,bilateral filter
* qrencode
