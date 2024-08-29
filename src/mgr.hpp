#include "sim.hpp"

#include <memory>

#include <madrona/py/utils.hpp>
#include <madrona/exec_mode.hpp>

#include <madrona/render/render_mgr.hpp>

namespace madRender {

// The Manager class encapsulates the linkage between the outside training
// code and the internal simulation state (src/sim.hpp / src/sim.cpp)
//
// Manager is responsible for initializing the simulator, loading physics
// and rendering assets off disk, and mapping ECS components to tensors
// for learning
class Manager {
public:
    struct Config {
        madrona::ExecMode execMode; // CPU or CUDA
        int gpuID; // Which GPU for CUDA backend?
        uint32_t numWorlds; // Simulation batch size
        bool enableBatchRenderer;
        uint32_t batchRenderViewWidth = 64;
        uint32_t batchRenderViewHeight = 64;
        madrona::render::APIBackend *extRenderAPI = nullptr;
        madrona::render::GPUDevice *extRenderDev = nullptr;
        uint32_t raycastOutputResolution = 64;
        bool headlessMode = false;

        // Render config
        struct RenderConfig {
            const char **assetPaths;
            uint32_t numAssetPaths;

            ImportedInstance *importedInstances;
            uint32_t numInstances;

            ImportedCamera *cameras;
            uint32_t numCameras;

            Sim::WorldInit *worlds;
        } rcfg;
    };

    Manager(const Config &cfg);
    ~Manager();

    void step();

    // These functions export Tensor objects that link the ECS
    // simulation state to the python bindings / PyTorch tensors (src/bindings.cpp)
    madrona::py::Tensor rgbTensor() const;
    madrona::py::Tensor depthTensor() const;
    madrona::py::Tensor raycastTensor() const;

    madrona::render::RenderManager & getRenderManager();

    uint32_t numAgents;

private:
    struct Impl;
    struct CUDAImpl;

    std::unique_ptr<Impl> impl_;
};

}
