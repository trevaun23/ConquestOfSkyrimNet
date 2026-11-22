@echo off
setlocal
rem Copies reference files (SkyrimNet API, docs, MCM base scripts) into .\ref so they can be read.
set "HERE=%~dp0"
set "DATA=C:\Program Files (x86)\The.Elder.Scrolls.V.Skyrim.Special.Edition.v1.6.1170\game\Data"
set "SN=%DATA%\SKSE\Plugins\SkyrimNet"
set "OUT=%HERE%ref"
if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%"
> "%HERE%fetch_log.txt" 2>&1 (
  for %%f in (SkyrimNetApi.psc skynet_Library.psc SKI_ConfigBase.psc MCM_ConfigBase.psc MCM.psc SKI_PlayerLoadGameAlias.psc SKI_QuestBase.psc) do (
    for /f "delims=" %%p in ('dir /s /b "%DATA%\%%f" 2^>nul') do copy /y "%%p" "%OUT%\"
  )
  robocopy "%SN%\docs" "%OUT%\sn_docs" /E /NFL /NDL /NJH /NJS
  robocopy "%SN%\library\skyrimnet.base\prompts\documentation" "%OUT%\sn_documentation" /E /NFL /NDL /NJH /NJS
  robocopy "%SN%\library\skyrimnet.base\prompts\memory" "%OUT%\sn_memory" /E /NFL /NDL /NJH /NJS
  robocopy "%SN%\library\skyrimnet.base\prompts\submodules\character_bio" "%OUT%\sn_character_bio" /E /NFL /NDL /NJH /NJS
  robocopy "%SN%\library\skyrimnet.base\prompts\components" "%OUT%\sn_components" /E /NFL /NDL /NJH /NJS
  copy /y "%SN%\library\skyrimnet.base\prompts\agent_knowledge_builder.prompt" "%OUT%\"
  copy /y "%SN%\library\skyrimnet.base\manifest.json" "%OUT%\base_manifest.json"
  copy /y "%SN%\library\skyrimnet.bios-miri-follower\manifest.json" "%OUT%\bios_manifest.json"
  copy /y "%SN%\library\skyrimnet.bios-miri-follower\install.json" "%OUT%\bios_install.json"
  copy /y "%SN%\content-registry.json" "%OUT%\"
  dir /s /b "%SN%\config" > "%OUT%\sn_config_tree.txt"
  dir /s /b "%SN%\jsonData" > "%OUT%\sn_jsondata_tree.txt"
  dir /s /b "%SN%\external" > "%OUT%\sn_external_tree.txt"
  dir /s /b "%DATA%\*.sknpack" > "%OUT%\sknpacks.txt"
  dir /s /b "%DATA%\MCM\Config" > "%OUT%\mcm_config_tree.txt"
  echo done
)
