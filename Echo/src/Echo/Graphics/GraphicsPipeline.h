#pragma once

namespace Echo
{
	struct GraphicsPipelineData 
	{
		const char* VertexShaderPath;
		const char* FragmentShaderPath;
	};

	class GraphicsPipeline
	{
	public:
		virtual ~GraphicsPipeline() {}

		virtual void Start() = 0;
		virtual void Finish() = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<GraphicsPipeline> Create(GraphicsPipelineData& data);
	};

	class PipelineLibrary
	{
	public:
		void Add(const Ref<GraphicsPipeline>& shader);
		void Add(const std::string& name, const Ref<GraphicsPipeline> shader);
		Ref<GraphicsPipeline> Load(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

		Ref<GraphicsPipeline> Get(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<GraphicsPipeline>> m_Pipelines;
	};
}