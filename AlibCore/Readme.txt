
【所有配置】 + 【所有平台】

【常规】
 1、输出目录
	$(SolutionDir)Lib\$(PlatformTarget)\$(Configuration)\

 2、中间目录
	$(SolutionDir)Tempout\$(PlatformTarget)\$(ProjectName)\$(Configuration)\

【C/C++】
 附加包含目录
	$(SolutionDir)Include;$(SolutionDir)$(ProjectName);$(SolutionDir)$(ProjectName)\src

【连接器】
 附加库目录
	$(SolutionDir)Lib\$(PlatformTarget)\$(Configuration)\

【创建 x.inl 和 x.h】


 AlibCore
 AlibGeos
 AlibMaths