
#include "XkslangDLL.h"
#include "../XkslParser/XkslParser.h"
#include "../XkfxProcessor/XkfxParser.h"
#include "../SpxMixer/SpxMixer.h"
#include "../Common/Converter.h"

using namespace std;
using namespace xkslang;
using namespace xkfxProcessor;

#pragma warning(disable:4996)  //disable annoying deprecation warnings

namespace xkslangDll
{
    static vector<string> errorMessages;
    static XkslParser* xkslParser = nullptr;

    static bool error(const string& errorMsg)
    {
        errorMessages.push_back(errorMsg);
        return false;
    }

    static bool error(const char* errorMsg)
    {
        //cout << "XkslangDLL Error: " << errorMsg << endl;
        if (errorMsg == 0 || strlen(errorMsg) == 0) return false;
        errorMessages.push_back(string(errorMsg));

        return false;
    }

	static char* allocateAndCopyStringOnGlobalHeap(const char* txt)
	{
		if (txt == nullptr) return nullptr;
		int len = strlen(txt);
        if (len == 0) return nullptr;

		char* res = (char*)GlobalAlloc(0, (len + 1) * sizeof(char));
		strncpy(res, txt, len);
		res[len] = '\0';
		return res;
	}

    char* GetErrorMessages()
    {
		if (errorMessages.size() == 0) return nullptr;

		stringstream stream;
        for (unsigned int n = 0; n < errorMessages.size(); ++n)
			stream << errorMessages[n] << endl;

		string str = stream.str();
		unsigned int len = str.size();

		//allocate a byte buffer using GlobalAlloc, so we can return it to the calling app and let it delete it
		char* pBuffer = allocateAndCopyStringOnGlobalHeap(str.c_str());
		return pBuffer;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    //Utility function to help converting a bytecode to a human-readable ascii file

    char* ConvertBytecodeToAsciiText(uint32_t* bytecode, int32_t bytecodeSize, int32_t* asciiBufferSize)
    {
        errorMessages.clear();
        if (bytecode == nullptr || asciiBufferSize == nullptr) { error("some parameters are null"); return nullptr; }
        *asciiBufferSize = -1;

        if (bytecode == nullptr || bytecodeSize <= 0) { error("bytecode is empty"); return nullptr; }

        string bytecodeText;
        std::vector<uint32_t> vecBytecode;
        vecBytecode.assign(bytecode, bytecode + bytecodeSize);

        if (!Converter::ConvertBytecodeToAsciiText(vecBytecode, bytecodeText)) {
            error("Failed to convert the bytecode to Ascii");
            return nullptr;
        }

        int asciiBufferLen = bytecodeText.size();
        if (asciiBufferLen == 0) { error("The Ascii buffer is empty"); return nullptr; }

        //allocate a byte buffer using GlobalAlloc, so we can return it to the calling app and let it delete it
        char* asciiBuffer = (char*)GlobalAlloc(0, asciiBufferLen * sizeof(char));
        strncpy(asciiBuffer, bytecodeText.c_str(), asciiBufferLen);
        *asciiBufferSize = asciiBufferLen;
        return asciiBuffer;
    }

    char* ConvertBytecodeToGlsl(uint32_t* bytecode, int32_t bytecodeSize, int32_t* asciiBufferSize)
    {
        errorMessages.clear();
        if (bytecode == nullptr || asciiBufferSize == nullptr) { error("some parameters are null"); return nullptr; }
        *asciiBufferSize = -1;

        if (bytecode == nullptr || bytecodeSize <= 0) { error("bytecode is empty"); return nullptr; }

        string bytecodeText;
        std::vector<uint32_t> vecBytecode;
        vecBytecode.assign(bytecode, bytecode + bytecodeSize);

        if (!Converter::ConvertBytecodeToGlsl(vecBytecode, bytecodeText)) {
            error("Failed to convert the bytecode to GLSL");
            return nullptr;
        }

        int asciiBufferLen = bytecodeText.size();
        if (asciiBufferLen == 0) { error("The Ascii buffer is empty"); return nullptr; }

		//allocate a byte buffer using GlobalAlloc, so we can return it to the calling app and let it delete it
        char* asciiBuffer = (char*)GlobalAlloc(0, asciiBufferLen * sizeof(char));
        strncpy(asciiBuffer, bytecodeText.c_str(), asciiBufferLen);
        *asciiBufferSize = asciiBufferLen;
        return asciiBuffer;
    }

    char* ConvertBytecodeToHlsl(uint32_t* bytecode, int32_t bytecodeSize, int32_t shaderModel, int32_t* asciiBufferSize)
    {
        errorMessages.clear();
        if (bytecode == nullptr || asciiBufferSize == nullptr) { error("some parameters are null"); return nullptr; }

        *asciiBufferSize = -1;

        if (bytecode == nullptr || bytecodeSize <= 0) { error("bytecode is empty"); return nullptr; }

        string bytecodeText;
        std::vector<uint32_t> vecBytecode;
        vecBytecode.assign(bytecode, bytecode + bytecodeSize);

        if (!Converter::ConvertBytecodeToHlsl(vecBytecode, shaderModel, bytecodeText)) {
            error("Failed to convert the bytecode to GLSL");
            return nullptr;
        }

        int asciiBufferLen = bytecodeText.size();
        if (asciiBufferLen == 0) { error("The Ascii buffer is empty"); return nullptr; }

		//allocate a byte buffer using GlobalAlloc, so we can return it to the calling app and let it delete it
        char* asciiBuffer = (char*)GlobalAlloc(0, asciiBufferLen * sizeof(char));
        strncpy(asciiBuffer, bytecodeText.c_str(), asciiBufferLen);
        *asciiBufferSize = asciiBufferLen;
        return asciiBuffer;
    }

    //Utility function Parsing the bytecode to return the names of shaders it contains
    bool GetBytecodeShadersInformation(uint32_t* bytecode, int32_t bytecodeSize, BytecodeShaderInformation** shadersInfo, int32_t* pcountShaders)
    {
        errorMessages.clear();
        if (bytecode == nullptr || shadersInfo == nullptr || pcountShaders == nullptr) { return error("some parameters are null"); }

        SpxBytecode spxBytecode;
        std::vector<uint32_t>& vecBytecode = spxBytecode.getWritableBytecodeStream();
        vecBytecode.assign(bytecode, bytecode + bytecodeSize);

        vector<string> vecShaderName;
        vector<string> errorMsgs;
        if (!SpxMixer::GetListAllShadersFromBytecode(spxBytecode, vecShaderName, errorMsgs) || false)
        {
            error("Failed to get the list of shader names from the bytecode");
            for (unsigned int k = 0; k < errorMsgs.size(); ++k) error(errorMsgs[k]);
            return false;
        }

        int countShaders = vecShaderName.size();
        if (countShaders == 0) return true;

        BytecodeShaderInformation* tabShadersInfo = (BytecodeShaderInformation*)GlobalAlloc(0, countShaders * sizeof(BytecodeShaderInformation));
        for (int k = 0; k < countShaders; ++k)
        {
            char* shaderName = allocateAndCopyStringOnGlobalHeap(vecShaderName[k].c_str());
            tabShadersInfo[k].ShaderName = shaderName;
        }
        *shadersInfo = tabShadersInfo;
        *pcountShaders = countShaders;

        return true;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    // Parsing and conversion functions

    bool InitializeParser()
    {
        if (xkslParser != nullptr) return error("Xkslang Parser has already been initialized");

        errorMessages.clear();

        xkslParser = new XkslParser();
        if (!xkslParser->InitialiseXkslang())
        {
            return error("Failed to initialize the Xkslang Parser");
        }

        return true;
    }

    void ReleaseParser()
    {
        if (xkslParser != nullptr)
        {
            xkslParser->Finalize();

            delete xkslParser;
            xkslParser = nullptr;
        }

        errorMessages.clear();
    }

    static ShaderSourceLoaderCallback externalShaderDataCallback = nullptr;
    //callback function given to xkslParser, doing the link between with the user, native callback function
    static bool callbackRequestDataForShader(const string& shaderName, string& shaderData)
    {
        if (externalShaderDataCallback == nullptr) return error("No callback function has been set");

        int dataLen;
        char* data = externalShaderDataCallback(shaderName.c_str(), &dataLen);
        if (data == nullptr || dataLen < 0) return false;

        shaderData.append(data);

        return true;
    }

    uint32_t* ConvertXkslShaderToSPX(const char* mainShaderName, const char* stringShadersWithGenerics, const char* stringMacrosDefinition,
        ShaderSourceLoaderCallback shaderDependencyCallback, int32_t* resultingBytecodeSize)
    {
        errorMessages.clear();
        *resultingBytecodeSize = 0;

        if (xkslParser == nullptr) {error("Xkslang parser has not been initialized"); return nullptr;}
        if (mainShaderName == nullptr) {error("shaderName is null"); return nullptr;}
        if (shaderDependencyCallback == nullptr) {error("The callback function is null"); return nullptr;}
        externalShaderDataCallback = shaderDependencyCallback;

        //Parse and get the list of shader with their generic values
        vector<ShaderParsingDefinition> listshaderDefinition;
        if (stringShadersWithGenerics != nullptr) {
            XkslParser::ParseStringWithShaderDefinitions(stringShadersWithGenerics, listshaderDefinition);
        }
        vector<ShaderGenericValues> listshaderWithGenerics;
        for (unsigned int is = 0; is < listshaderDefinition.size(); is++) {
            listshaderWithGenerics.push_back(ShaderGenericValues(listshaderDefinition[is].shaderName, listshaderDefinition[is].genericsValue));
        }

        //Get the list of macro and their definitions
        vector<XkslUserDefinedMacro> listUserDefinedMacros;
        if (stringMacrosDefinition != nullptr)
        {
            if (XkslParser::ParseStringMacroDefinition(stringMacrosDefinition, listUserDefinedMacros, true) == -1)
            {
                error("Failed to parse the macro definition string");
                return nullptr;
            }
        }

        SpxBytecode spirXBytecode;
        ostringstream errorMessages;
        bool success = xkslParser->ConvertShaderToSpx(mainShaderName, callbackRequestDataForShader, listshaderWithGenerics, listUserDefinedMacros, spirXBytecode, &errorMessages);

        if (!success) {
            error(errorMessages.str().c_str());
            return nullptr;
        }

        const vector<uint32_t> bytecode = spirXBytecode.getBytecodeStream();
        int bytecodeLen = (int)bytecode.size();
        if (bytecodeLen <= 0) { error("Resulting bytecode is empty"); return nullptr; }

		//allocate a byte buffer using GlobalAlloc, so we can return it to the calling app and let it delete it
        uint32_t* byteBuffer = (uint32_t*)GlobalAlloc(0, bytecodeLen * sizeof(uint32_t));
        //for (int i = 0; i < bytecodeLen; ++i) byteBuffer[i] = bytecode[i];
        
        uint32_t* pDest = byteBuffer;
        const uint32_t* pSrc = &bytecode[0];
        int countIteration = bytecodeLen;
        while (countIteration-- > 0) *pDest++ = *pSrc++;

        *resultingBytecodeSize = bytecodeLen;
        return byteBuffer;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    // Mixin functions

    //A mixer and its outputs bytecode
    class MixerData
    {
    public:
        SpxMixer* mixer;

		//Store compilation results
		bool compilationDone;
		SpvBytecode finalCompiledSpv;
		vector<OutputStageBytecode> stagesCompiledData;

		//Store EffectReflection
		bool effectReflectionDone;
		EffectReflection effectReflection;

        MixerData() : mixer(nullptr), compilationDone(false), effectReflectionDone(false) {}
        MixerData(SpxMixer* mixer) : mixer(mixer), compilationDone(false), effectReflectionDone(false) {}
        ~MixerData() { if (mixer != nullptr) delete mixer; }
    };
    static std::vector<MixerData*> listMixerData;

    bool InitializeMixer()
    {
        SpxMixer::StartMixinEffect();
        return true;
    }

    void ReleaseMixer()
    {
        SpxMixer::StopMixinEffect();
    }

    static MixerData* GetMixerForHandleId(uint32_t mixerHandleId)
    {
        uint32_t index = mixerHandleId - 1;
        if (mixerHandleId == 0 || index >= listMixerData.size()) return nullptr;

        MixerData* mixerData = listMixerData[index];
        if (mixerData == nullptr) return nullptr;

        return mixerData;
    }

    uint32_t CreateSpxShaderMixer()
    {
        SpxMixer* mixer = new SpxMixer();
        MixerData* mixerData = new MixerData(mixer);

        uint32_t handleId = listMixerData.size();
        listMixerData.push_back(mixerData);

        return handleId + 1;
    }

    bool ReleaseSpxShaderMixer(uint32_t mixerHandleId)
    {
        errorMessages.clear();

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr)
            return error("Invalid mixer handle");

        uint32_t index = mixerHandleId - 1;
        if (mixerHandleId == 0 || index >= listMixerData.size())
            return error("Invalid mixer handle");

        delete mixerData;
        listMixerData[index] = nullptr;

        int indexLastValidElement = listMixerData.size() - 1;
        while (indexLastValidElement >= 0 && listMixerData[indexLastValidElement] == nullptr) indexLastValidElement--;

        if (indexLastValidElement != listMixerData.size() - 1)
        {
            if (indexLastValidElement < 0) listMixerData.clear();
            else listMixerData.resize(indexLastValidElement + 1);
        }

        return true;
    }

    bool AddComposition(uint32_t mixerHandleId, const char* shaderName, const char* variableName, uint32_t compositionMixerHandleId)
    {
        errorMessages.clear();
        if (shaderName == nullptr || variableName == nullptr) { error("Invalid parameters"); return false; }

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr || mixerData->mixer == nullptr) return error("Invalid mixer handle");
        SpxMixer* mixer = mixerData->mixer;

        MixerData* compositionMixerData = GetMixerForHandleId(compositionMixerHandleId);
        if (compositionMixerData == nullptr) return error("Invalid composition mixer handle");
        SpxMixer* compositionMixer = compositionMixerData->mixer;

        vector<string> errorMsgs;
        bool success = mixer->AddCompositionInstance(shaderName, variableName, compositionMixer, errorMsgs);

        if (!success)
        {
            for (unsigned int k = 0; k < errorMsgs.size(); ++k) error(errorMsgs[k]);
        }

        return success;
    }

    bool MixinShaders(uint32_t mixerHandleId, const char* stringShadersWithGenerics, uint32_t* shaderSpxBytecode, int32_t bytecodeSize)
    {
        errorMessages.clear();
        if (stringShadersWithGenerics == nullptr) { error("List of shaders is empty"); return false; }
        if (shaderSpxBytecode == nullptr || bytecodeSize <= 0) { error("bytecode is empty"); return false; }

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr || mixerData->mixer == nullptr) return error("Invalid mixer handle");
        SpxMixer* mixer = mixerData->mixer;

        //Parse and get the list of shader with their generic values
        vector<ShaderParsingDefinition> listshaderDefinition;
        if (stringShadersWithGenerics != nullptr) {
            XkslParser::ParseStringWithShaderDefinitions(stringShadersWithGenerics, listshaderDefinition);
        }
        vector<ShaderGenericValues> listshaderWithGenerics;
        for (unsigned int is = 0; is < listshaderDefinition.size(); is++) {
            listshaderWithGenerics.push_back(ShaderGenericValues(listshaderDefinition[is].shaderName, listshaderDefinition[is].genericsValue));
        }
        if (listshaderWithGenerics.size() == 0) return error("No shader found for mixin");

        vector<string> listShadersName;
        for (unsigned int k = 0; k < listshaderWithGenerics.size(); k++) {
            listShadersName.push_back(listshaderWithGenerics[k].GetShaderNameWithGenerics());
        }

        //copy the shaders bytecode to mix
        SpxBytecode spxBytecode(listShadersName[0]);
        std::vector<uint32_t>& bytecode = spxBytecode.getWritableBytecodeStream();
        bytecode.insert(bytecode.end(), shaderSpxBytecode, shaderSpxBytecode + bytecodeSize);

        //proceed with the mixin
        vector<string> errorMsgs;
        bool success = mixer->Mixin(spxBytecode, listShadersName, errorMsgs);
        if (!success) {
            for (unsigned int k = 0; k < errorMsgs.size(); ++k) error(errorMsgs[k]);
        }

        return success;
    }

    bool ReleaseMixinHandle(uint32_t mixerHandleId)
    {
        return ReleaseSpxShaderMixer(mixerHandleId);
    }

    uint32_t ExecuteEffectCommandLines(const char* effectCommandLine, ShaderSourceLoaderCallback shaderDependencyCallback)
    {
        errorMessages.clear();
        if (effectCommandLine == nullptr) { error("Command line is missing"); return 0; }
        if (xkslParser == nullptr) { error("Xkslang parser has not been initialized"); return 0; }

        //create a dummy mixer object to hold the data
        MixerData* mixerData = new MixerData();

        //set the callback function
        externalShaderDataCallback = shaderDependencyCallback;

        vector<string> errorMsgs;
        vector<uint32_t>* compiledBytecode = &(mixerData->finalCompiledSpv.bytecode);
        vector<OutputStageBytecode>& outputStages = mixerData->stagesCompiledData;
        bool success = XkfxParser::ProcessXkfxCommandLines(xkslParser, effectCommandLine, callbackRequestDataForShader, compiledBytecode, outputStages, errorMsgs);

        if (!success) {
            for (unsigned int k = 0; k < errorMsgs.size(); ++k) error(errorMsgs[k]);
            delete mixerData;
            return 0;
        }

        mixerData->compilationDone = true;

        uint32_t mixerHandleId = listMixerData.size();
        listMixerData.push_back(mixerData);
        return mixerHandleId + 1;
    }

    bool CompileMixer(uint32_t mixerHandleId, OutputStageEntryPoint* stageEntryPointArray, int32_t countStages)
    {
        errorMessages.clear();

        if (countStages <= 0) return error("Invalid number of output stages");

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr || mixerData->mixer == nullptr) return error("Invalid mixer handle");
        SpxMixer* mixer = mixerData->mixer;

        for (int s = 0; s < countStages; s++)
        {
            if (!IsAValidOutputStage(stageEntryPointArray[s].stage)) return error("Output stage is not valid: " + to_string((int)(stageEntryPointArray[s].stage)));
            mixerData->stagesCompiledData.push_back(OutputStageBytecode(stageEntryPointArray[s].stage, string(stageEntryPointArray[s].entryPointName)));
        }

        vector<string> errorMsgs;
		SpvBytecode& finalSpv = mixerData->finalCompiledSpv;
        std::vector<uint32_t>* pCompiledBytecode = &(finalSpv.bytecode);

        //set the mixer's stages to compile
        mixerData->stagesCompiledData.clear();
        for (int s = 0; s < countStages; s++)
        {
            if (!IsAValidOutputStage(stageEntryPointArray[s].stage)) return error("Output stage is not valid: " + to_string((int)(stageEntryPointArray[s].stage)));
            mixerData->stagesCompiledData.push_back(OutputStageBytecode(stageEntryPointArray[s].stage, string(stageEntryPointArray[s].entryPointName)));
        }

		mixerData->compilationDone = false;
		mixerData->effectReflectionDone = false;

        bool success = mixer->Compile(mixerData->stagesCompiledData, errorMsgs, nullptr, nullptr, nullptr, nullptr, pCompiledBytecode, nullptr);
        if (!success)
        {
            for (unsigned int k = 0; k < errorMsgs.size(); ++k)
                error(errorMsgs[k]);
            return false;
        }

		mixerData->compilationDone = true;
        return true;
    }

    bool GetMixerMethodsData(uint32_t mixerHandleId, MethodData** p_methods, int32_t* p_countMethods)
    {
        errorMessages.clear();

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr || mixerData->mixer == nullptr) { return error("Invalid mixer handle"); }
        
        vector<string> errorMsgs;
        vector<MethodInfo> vecMethods;
        bool success = mixerData->mixer->GetListAllMethodsInfo(vecMethods, errorMsgs);
        if (!success) { return error("Failed to get the list of all methods from the mixer"); }

        unsigned int countMethods = vecMethods.size();
        if (countMethods > 0)
        {
            MethodData* methodsBuffer = (MethodData*)GlobalAlloc(0, countMethods * sizeof(MethodData));

            for (unsigned int k = 0; k < countMethods; k++)
            {
                const MethodInfo& method = vecMethods[k];

                const char* methodName = allocateAndCopyStringOnGlobalHeap(method.Name.c_str());
                const char* shaderClassName = allocateAndCopyStringOnGlobalHeap(method.ShaderClassName.c_str());
                methodsBuffer[k] = MethodData(
                    methodName,
                    shaderClassName,
                    method.IsStage? 1: 0
                );
            }
            *p_methods = methodsBuffer;
        }
        else *p_methods = nullptr;

        *p_countMethods = (int32_t)countMethods;

        return true;
    }

    static void ConvertStructMemberToDllData(const TypeMemberReflectionDescription& structMemberSrc, ConstantBufferMemberReflectionDescriptionData& structMemberDst)
    {
        const char* memberName = allocateAndCopyStringOnGlobalHeap(structMemberSrc.Name.c_str());

        structMemberDst.Set(
            structMemberSrc.Offset,
            memberName,
            nullptr,
            nullptr,
            structMemberSrc.Type.Class,
            structMemberSrc.Type.Type,
            structMemberSrc.Type.RowCount,
            structMemberSrc.Type.ColumnCount,
            structMemberSrc.Type.Size,
            structMemberSrc.Type.Alignment,
            structMemberSrc.Type.ArrayStride,
            structMemberSrc.Type.MatrixStride,
            structMemberSrc.Type.ArrayElements,
            structMemberSrc.Type.CountMembers
        );

        if (structMemberSrc.Type.Class == EffectParameterReflectionClass::Struct)
        {
            int structMemberCountSubMembers = structMemberSrc.Type.CountMembers;
            if (structMemberCountSubMembers > 0 && structMemberSrc.Type.Members != nullptr)
            {
                //create the struct sub members details
                ConstantBufferMemberReflectionDescriptionData* structMemberSubStruct = nullptr;
                structMemberSubStruct = (ConstantBufferMemberReflectionDescriptionData*)GlobalAlloc(0, structMemberCountSubMembers * sizeof(ConstantBufferMemberReflectionDescriptionData));

                for (int sm = 0; sm < structMemberCountSubMembers; ++sm)
                {
                    const TypeMemberReflectionDescription& structSubMemberSrc = structMemberSrc.Type.Members[sm];
                    ConstantBufferMemberReflectionDescriptionData& structSubMemberDst = structMemberSubStruct[sm];
                    ConvertStructMemberToDllData(structSubMemberSrc, structSubMemberDst);
                }

                structMemberDst.SetMembersDetails(structMemberSubStruct, structMemberCountSubMembers);
            }
        }
    }

	bool GetMixerEffectReflectionData(uint32_t mixerHandleId,
		ConstantBufferReflectionDescriptionData** constantBuffers, int32_t* countConstantBuffers,
		EffectResourceBindingDescriptionData** resourceBindings, int32_t* countResourceBindings,
		ShaderInputAttributeDescriptionData** inputAttributes, int32_t* countInputAttributes)
	{
		errorMessages.clear();

		MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
		if (mixerData == nullptr) { return error("Invalid mixer handle"); }
		if (!mixerData->compilationDone) { return error("The mixer has not been compiled"); }

		if (!mixerData->effectReflectionDone)
		{
			SpvBytecode& compiledBytecode = mixerData->finalCompiledSpv;
			bool success = SpxMixer::GetCompiledBytecodeReflection(compiledBytecode.bytecode, mixerData->effectReflection, errorMessages);
			if (!success)
			{
				return error("Failed to get the reflection data from the compiled bytecode");
			}

			mixerData->effectReflectionDone = true;
		}

		const EffectReflection& effectReflectionSrc = mixerData->effectReflection;

		//===========================================================================================================================================================================
		//===========================================================================================================================================================================
		//Copy the effect reflection data into the output parameters: allocate all buffers & elements using GlobalAlloc, so that the calling framework can delete it properly

		//ConstantBuffers
		if (constantBuffers != nullptr && countConstantBuffers != nullptr)
		{
			*countConstantBuffers = effectReflectionSrc.CountConstantBuffers;
			if (effectReflectionSrc.CountConstantBuffers > 0)
			{
				ConstantBufferReflectionDescriptionData* arrayConstantBuffer = (ConstantBufferReflectionDescriptionData*)GlobalAlloc(0, effectReflectionSrc.CountConstantBuffers * sizeof(ConstantBufferReflectionDescriptionData));
				for (int k = 0; k < effectReflectionSrc.CountConstantBuffers; ++k)
				{
					const ConstantBufferReflectionDescription& constantBufferSrc = effectReflectionSrc.ConstantBuffers[k];
					
					int countMembers = constantBufferSrc.Members.size();
					ConstantBufferMemberReflectionDescriptionData* membersInfo = nullptr;
					membersInfo = (ConstantBufferMemberReflectionDescriptionData*)GlobalAlloc(0, countMembers * sizeof(ConstantBufferMemberReflectionDescriptionData));
					for (int m = 0; m < countMembers; ++m)
					{
						const ConstantBufferMemberReflectionDescription& memberSrc = constantBufferSrc.Members[m];
						const char* memberKeyName = allocateAndCopyStringOnGlobalHeap(memberSrc.KeyName.c_str());
                        const char* memberRawName = allocateAndCopyStringOnGlobalHeap(memberSrc.RawName.c_str());
                        const char* logicalGroup = allocateAndCopyStringOnGlobalHeap(memberSrc.LogicalGroup.c_str());
						membersInfo[m] = ConstantBufferMemberReflectionDescriptionData(
							memberSrc.Offset,
                            memberKeyName,
                            memberRawName,
                            logicalGroup,
							memberSrc.ReflectionType
						);

                        if (memberSrc.ReflectionType.Class == EffectParameterReflectionClass::Struct)
                        {
                            int structCountMembers = memberSrc.ReflectionType.CountMembers;
                            if (structCountMembers > 0 && memberSrc.ReflectionType.Members != nullptr)
                            {
                                //create the struct sub members details
                                ConstantBufferMemberReflectionDescriptionData* structMembers = nullptr;
                                structMembers = (ConstantBufferMemberReflectionDescriptionData*)GlobalAlloc(0, structCountMembers * sizeof(ConstantBufferMemberReflectionDescriptionData));

                                for (int sm = 0; sm < structCountMembers; ++sm)
                                {
                                    const TypeMemberReflectionDescription& structMemberSrc = memberSrc.ReflectionType.Members[sm];
                                    ConstantBufferMemberReflectionDescriptionData& structMemberDst = structMembers[sm];
                                    ConvertStructMemberToDllData(structMemberSrc, structMemberDst);
                                }

                                membersInfo[m].SetMembersDetails(structMembers, structCountMembers);
                            }
                        }
					}

					const char* cbufferName = allocateAndCopyStringOnGlobalHeap(constantBufferSrc.CbufferName.c_str());
					arrayConstantBuffer[k] = ConstantBufferReflectionDescriptionData(
						constantBufferSrc.Size,
						countMembers,
						cbufferName,
						membersInfo
					);
				}
				*constantBuffers = arrayConstantBuffer;
			}
			else
			{
				*constantBuffers = nullptr;
			}
		}

		//ResourceBinding
		if (resourceBindings != nullptr && countResourceBindings != nullptr)
		{
			*countResourceBindings = effectReflectionSrc.CountResourceBindings;
			if (effectReflectionSrc.CountResourceBindings > 0)
			{
				EffectResourceBindingDescriptionData* arrayResourceBindings = (EffectResourceBindingDescriptionData*)GlobalAlloc(0, effectReflectionSrc.CountResourceBindings * sizeof(EffectResourceBindingDescriptionData));
				for (int k = 0; k < effectReflectionSrc.CountResourceBindings; ++k)
				{
					const char* keyName = allocateAndCopyStringOnGlobalHeap(effectReflectionSrc.ResourceBindings[k].KeyName.c_str());
                    const char* rawName = allocateAndCopyStringOnGlobalHeap(effectReflectionSrc.ResourceBindings[k].RawName.c_str());
					arrayResourceBindings[k] = EffectResourceBindingDescriptionData(effectReflectionSrc.ResourceBindings[k], keyName, rawName);
				}
				*resourceBindings = arrayResourceBindings;
			}
			else
			{
				*resourceBindings = nullptr;
			}
		}

		//Input attributes
		if (inputAttributes != nullptr && countInputAttributes != nullptr)
		{
			*countInputAttributes = effectReflectionSrc.CountInputAttributes;
			if (effectReflectionSrc.CountInputAttributes > 0)
			{
				ShaderInputAttributeDescriptionData* arrayInputAttributes = (ShaderInputAttributeDescriptionData*)GlobalAlloc(0, effectReflectionSrc.CountInputAttributes * sizeof(ShaderInputAttributeDescriptionData));
				for (int k = 0; k < effectReflectionSrc.CountInputAttributes; ++k)
				{
					const char* semanticName = allocateAndCopyStringOnGlobalHeap(effectReflectionSrc.InputAttributes[k].SemanticName.c_str());
					arrayInputAttributes[k] = ShaderInputAttributeDescriptionData(semanticName, effectReflectionSrc.InputAttributes[k].SemanticIndex);
				}
				*inputAttributes = arrayInputAttributes;
			}
			else
			{
				*inputAttributes = nullptr;
			}
		}

		return true;
	}

    uint32_t* GetMixerCurrentBytecode(uint32_t mixerHandleId, int32_t* bytecodeSize)
    {
        errorMessages.clear();
        *bytecodeSize = 0;

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr || mixerData->mixer == nullptr) { error("Invalid mixer handle"); return nullptr; }

        const vector<uint32_t>* mixerBytecode = mixerData->mixer->GetCurrentMixinBytecode();
        if (mixerBytecode == nullptr) { error("Failed to get the mixer bytecode"); return nullptr; }

        /// copy the bytecode into the output buffer: allocate a byte buffer using GlobalAlloc, so we can return it to the calling framework and let it delete it
        int bytecodeLen = (int)mixerBytecode->size();
        if (bytecodeLen <= 0) { error("the mixer compiled bytecode is empty"); return nullptr; }

        uint32_t* byteBuffer = (uint32_t*)GlobalAlloc(0, bytecodeLen * sizeof(uint32_t));

        uint32_t* pDest = byteBuffer;
        const uint32_t* pSrc = &(mixerBytecode->front());
        int countIteration = bytecodeLen;
        while (countIteration-- > 0) *pDest++ = *pSrc++;

        *bytecodeSize = bytecodeLen;
        return byteBuffer;
    }

    int32_t GetMixerCurrentBytecodeSize(uint32_t mixerHandleId)
    {
        errorMessages.clear();

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr || mixerData->mixer == nullptr) { error("Invalid mixer handle"); return 0; }

        const vector<uint32_t>* mixerBytecode = mixerData->mixer->GetCurrentMixinBytecode();
        if (mixerBytecode == nullptr) { error("Failed to get the mixer bytecode"); return 0; }

        return (uint32_t)mixerBytecode->size();
    }
    
    int32_t CopyMixerCurrentBytecode(uint32_t mixerHandleId, uint32_t* bytecodeBuffer, int32_t bufferSize)
    {
        errorMessages.clear();

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr || mixerData->mixer == nullptr) { error("Invalid mixer handle"); return 0; }

        const vector<uint32_t>* mixerBytecode = mixerData->mixer->GetCurrentMixinBytecode();
        if (mixerBytecode == nullptr) { error("Failed to get the mixer bytecode"); return 0; }

        int bytecodeLen = mixerBytecode->size();
        if (bytecodeLen <= 0) { error("The mixer current bytecode is empty"); return 0; }
        if (bytecodeLen < bufferSize) { error("The given bytecode buffer has an invalid size. Expected (at least): " + to_string(bytecodeLen)); return 0; }

        uint32_t* pDest = bytecodeBuffer;
        const uint32_t* pSrc = &(mixerBytecode->front());
        int countIteration = bytecodeLen;
        while (countIteration-- > 0) *pDest++ = *pSrc++;

        return bytecodeLen;
    }

	uint32_t* GetMixerCompiledBytecode(uint32_t mixerHandleId, int32_t* bytecodeSize)
	{
		errorMessages.clear();
		*bytecodeSize = 0;

		MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
		if (mixerData == nullptr) { error("Invalid mixer handle"); return nullptr; }
		if (!mixerData->compilationDone) { error("The mixer has not been compiled"); return nullptr; }

		SpvBytecode& compiledBytecode = mixerData->finalCompiledSpv;

		/// copy the bytecode into the output buffer: allocate a byte buffer using GlobalAlloc, so we can return it to the calling framework and let it delete it
		const std::vector<uint32_t>& bytecode = compiledBytecode.getBytecodeStream();
		int bytecodeLen = (int)bytecode.size();
		if (bytecodeLen <= 0) { error("the mixer compiled bytecode is empty"); return nullptr; }

		uint32_t* byteBuffer = (uint32_t*)GlobalAlloc(0, bytecodeLen * sizeof(uint32_t));

		uint32_t* pDest = byteBuffer;
		const uint32_t* pSrc = &bytecode[0];
		int countIteration = bytecodeLen;
		while (countIteration-- > 0) *pDest++ = *pSrc++;

		*bytecodeSize = bytecodeLen;
		return byteBuffer;
	}

	int32_t GetMixerCompiledBytecodeSize(uint32_t mixerHandleId)
	{
		errorMessages.clear();

		MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
		if (mixerData == nullptr) { error("Invalid mixer handle"); return 0; }
		if (!mixerData->compilationDone) { error("The mixer has not been compiled"); return 0; }

		SpvBytecode& compiledBytecode = mixerData->finalCompiledSpv;
		int bytecodeLen = compiledBytecode.GetBytecodeSize();
		if (bytecodeLen <= 0) { error("The mixer compiled bytecode is empty"); return 0; }

		return bytecodeLen;
	}

	int32_t CopyMixerCompiledBytecode(uint32_t mixerHandleId, uint32_t* bytecodeBuffer, int32_t bufferSize)
	{
		errorMessages.clear();

		MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
		if (mixerData == nullptr) { error("Invalid mixer handle"); return 0; }
		if (!mixerData->compilationDone) { error("The mixer has not been compiled"); return 0; }

		SpvBytecode& compiledBytecode = mixerData->finalCompiledSpv;
		const std::vector<uint32_t>& bytecode = compiledBytecode.getBytecodeStream();
		int bytecodeLen = bytecode.size();
		if (bytecodeLen <= 0) { error("The mixer compiled bytecode is empty"); return 0; }
		if (bytecodeLen < bufferSize) { error("The given bytecode buffer has an invalid size. Expected (at least): " + to_string(bytecodeLen)); return 0; }

		uint32_t* pDest = bytecodeBuffer;
		const uint32_t* pSrc = &bytecode[0];
		int countIteration = bytecodeLen;
		while (countIteration-- > 0) *pDest++ = *pSrc++;

		return bytecodeLen;
	}

    int32_t GetMixerCountCompiledStages(uint32_t mixerHandleId)
    {
        errorMessages.clear();
        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr) { error("Invalid mixer handle"); return 0; }
        if (!mixerData->compilationDone) { error("The mixer has not been compiled"); return 0; }

        int32_t countCompiledStages = (int32_t)mixerData->stagesCompiledData.size();
        return countCompiledStages;
    }

    uint32_t* GetMixerCompiledBytecodeForStageNum(uint32_t mixerHandleId, uint32_t stageNum, xkslang::ShadingStageEnum* stage, int32_t* bytecodeSize)
    {
        errorMessages.clear();
        if (bytecodeSize == nullptr) { error("Some parameters are null"); return nullptr; }
        *bytecodeSize = 0;

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr) { error("Invalid mixer handle"); return nullptr; }
        if (!mixerData->compilationDone) { error("The mixer has not been compiled"); return nullptr; }

        uint32_t countCompiledStages = (uint32_t)mixerData->stagesCompiledData.size();
        if (stageNum >= countCompiledStages) { error("Invalid stage num"); return nullptr; }

        OutputStageBytecode* outputStageBytecode = &(mixerData->stagesCompiledData[stageNum]);      
        if (outputStageBytecode == nullptr || outputStageBytecode->resultingBytecode.GetBytecodeSize() == 0) { error("The mixer has not been compiled for the given stage"); return nullptr; }

        /// copy the bytecode into the output buffer: allocate a byte buffer using GlobalAlloc, so we can return it to the calling framework and let it delete it
        const std::vector<uint32_t>& bytecode = outputStageBytecode->resultingBytecode.getBytecodeStream();
        int bytecodeLen = (int)bytecode.size();
        if (bytecodeLen <= 0) { error("the stage compiled bytecode is empty"); return nullptr; }

        uint32_t* byteBuffer = (uint32_t*)GlobalAlloc(0, bytecodeLen * sizeof(uint32_t));

        uint32_t* pDest = byteBuffer;
        const uint32_t* pSrc = &bytecode[0];
        int countIteration = bytecodeLen;
        while (countIteration-- > 0) *pDest++ = *pSrc++;

        if (stage != nullptr) *stage = outputStageBytecode->stage;
        *bytecodeSize = bytecodeLen;
        return byteBuffer;
    }

    uint32_t* GetMixerCompiledBytecodeForStage(uint32_t mixerHandleId, xkslang::ShadingStageEnum stage, int32_t* bytecodeSize)
    {
        errorMessages.clear();
        if (bytecodeSize == nullptr) { error("Some parameters are null"); return nullptr; }
        *bytecodeSize = 0;

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr) {error("Invalid mixer handle"); return nullptr;}
		if (!mixerData->compilationDone) { error("The mixer has not been compiled"); return nullptr; }

        unsigned int countStages = mixerData->stagesCompiledData.size();
        uint32_t stageNum = countStages;
        for (unsigned int k = 0; k < countStages; k++) {
            if (mixerData->stagesCompiledData[k].stage == stage) {
                stageNum = k;
                break;
            }
        }
        if (stageNum == countStages) {
            error("The mixer has not been compiled for the given stage");
            return nullptr;
        }

        return GetMixerCompiledBytecodeForStageNum(mixerHandleId, stageNum, nullptr, bytecodeSize);
    }

    int32_t GetMixerCompiledBytecodeSizeForStageNum(uint32_t mixerHandleId, uint32_t stageNum, xkslang::ShadingStageEnum* stage)
    {
        errorMessages.clear();

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr) { error("Invalid mixer handle"); return 0; }
        if (!mixerData->compilationDone) { error("The mixer has not been compiled"); return 0; }

        uint32_t countCompiledStages = (uint32_t)mixerData->stagesCompiledData.size();
        if (stageNum >= countCompiledStages) { error("Invalid stage num"); return 0; }

        OutputStageBytecode* outputStageBytecode = &(mixerData->stagesCompiledData[stageNum]);
        if (outputStageBytecode == nullptr || outputStageBytecode->resultingBytecode.GetBytecodeSize() == 0) { error("The mixer has not been compiled for the given stage"); return 0; }

        const std::vector<uint32_t>& bytecode = outputStageBytecode->resultingBytecode.getBytecodeStream();
        int32_t bytecodeLen = bytecode.size();

        if (stage != nullptr) *stage = outputStageBytecode->stage;
        return bytecodeLen;
    }

	int32_t GetMixerCompiledBytecodeSizeForStage(uint32_t mixerHandleId, xkslang::ShadingStageEnum stage)
    {
        errorMessages.clear();

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr) { error("Invalid mixer handle"); return 0; }
		if (!mixerData->compilationDone) { error("The mixer has not been compiled"); return 0; }

        unsigned int countStages = mixerData->stagesCompiledData.size();
        uint32_t stageNum = countStages;
        for (unsigned int k = 0; k < countStages; k++) {
            if (mixerData->stagesCompiledData[k].stage == stage) {
                stageNum = k;
                break;
            }
        }
        if (stageNum == countStages) {
            error("The mixer has not been compiled for the given stage");
            return 0;
        }

        return GetMixerCompiledBytecodeSizeForStageNum(mixerHandleId, stageNum, nullptr);
    }

    int32_t CopyMixerCompiledBytecodeForStageNum(uint32_t mixerHandleId, uint32_t stageNum, xkslang::ShadingStageEnum* stage, uint32_t* bytecodeBuffer, int32_t bufferSize)
    {
        errorMessages.clear();

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr) { error("Invalid mixer handle"); return 0; }
        if (!mixerData->compilationDone) { error("The mixer has not been compiled"); return 0; }

        uint32_t countCompiledStages = (uint32_t)mixerData->stagesCompiledData.size();
        if (stageNum >= countCompiledStages) { error("Invalid stage num"); return 0; }
        OutputStageBytecode* outputStageBytecode = &(mixerData->stagesCompiledData[stageNum]);

        if (outputStageBytecode == nullptr || outputStageBytecode->resultingBytecode.GetBytecodeSize() == 0) {
            error("The mixer has not been compiled for the given stage");
            return 0;
        }

        const std::vector<uint32_t>& bytecode = outputStageBytecode->resultingBytecode.getBytecodeStream();
        int bytecodeLen = bytecode.size();
        if (bytecodeLen <= 0) { error("the stage compiled bytecode is empty"); return 0; }
        if (bytecodeLen < bufferSize) { error("The given bytecode buffer has an invalid size. Expected (at least): " + to_string(bytecodeLen)); return 0; }

        uint32_t* pDest = bytecodeBuffer;
        const uint32_t* pSrc = &bytecode[0];
        int countIteration = bytecodeLen;
        while (countIteration-- > 0) *pDest++ = *pSrc++;

        if (stage != nullptr) *stage = outputStageBytecode->stage;
        return bytecodeLen;
    }

	int32_t CopyMixerCompiledBytecodeForStage(uint32_t mixerHandleId, xkslang::ShadingStageEnum stage, uint32_t* bytecodeBuffer, int32_t bufferSize)
    {
        errorMessages.clear();

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr) { error("Invalid mixer handle"); return 0; }
		if (!mixerData->compilationDone) { error("The mixer has not been compiled"); return 0; }

        unsigned int countStages = mixerData->stagesCompiledData.size();
        uint32_t stageNum = countStages;
        for (unsigned int k = 0; k < countStages; k++) {
            if (mixerData->stagesCompiledData[k].stage == stage) {
                stageNum = k;
                break;
            }
        }
        if (stageNum == countStages) {
            error("The mixer has not been compiled for the given stage");
            return 0;
        }

        return CopyMixerCompiledBytecodeForStageNum(mixerHandleId, stageNum, nullptr, bytecodeBuffer, bufferSize);
    }
}
