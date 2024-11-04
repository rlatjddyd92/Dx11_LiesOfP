

xcopy	/y		..\Bin\Engine.lib				..\..\EngineSDK\Lib\
xcopy	/y		..\ThirdPartyLib\*.lib			..\..\EngineSDK\Lib\

xcopy	/y		..\Bin\Engine.dll				..\..\Client\Bin\
xcopy	/y		..\Bin\ShaderFiles\*.*			..\..\Client\Bin\ShaderFiles\
xcopy	/y/s		..\Public\*.*				..\..\EngineSDK\Inc\

xcopy	/y		..\Bin\Engine.dll				..\..\Tools\Bin\
xcopy	/y		..\Bin\ShaderFiles\*.*			..\..\Tools\Bin\ShaderFiles\