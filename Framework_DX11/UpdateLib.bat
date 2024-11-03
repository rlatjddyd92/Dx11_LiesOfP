

xcopy	/y		.\Engine\Bin\Engine.lib				.\EngineSDK\Lib\
xcopy	/y		.\Engine\ThirdPartyLib\*.lib			.\EngineSDK\Lib\

xcopy	/y		.\Engine\Bin\Engine.dll				.\Client\Bin\
xcopy	/y		.\Engine\Bin\ShaderFiles\*.*			.\Client\Bin\ShaderFiles\
xcopy	/y/s		.\Engine\Public\*.*				.\EngineSDK\Inc\

xcopy	/y		.\Engine\Bin\Engine.dll				.\Tools\Bin\
xcopy	/y		.\Engine\Bin\ShaderFiles\*.*			.\Tools\Bin\ShaderFiles\