#pragma once

#include "UnmanagedWrapper.h"
#include "core/core.h"
#include "Vanguard/VanguardClient.h"
#include <chrono>

#include "video_core/renderer_base.h"

//C++/CLI has various restrictions (no std::mutex for example), so we can't actually include certain headers directly
//What we CAN do is wrap those functions

bool UnmanagedWrapper::savestate_done = false;

void UnmanagedWrapper::VANGUARD_EXIT() {
    //wxGetApp().Exit();
}

std::string GetSaveStatePath(u64 program_id, u32 slot) {
    return fmt::format("{}{:016X}.{:02d}.cst", FileUtil::GetUserPath(FileUtil::UserPath::StatesDir),
                       program_id, slot);
}


bool UnmanagedWrapper::IS_N3DS() {
    return Settings::values.is_new_3ds ;
}
void UnmanagedWrapper::SET_N3DS(bool isN3DS) {
    Settings::values.is_new_3ds = isN3DS;
}

std::string UnmanagedWrapper::VANGUARD_GETGAMENAME() {
    std::string title;
    Core::System::GetInstance().GetAppLoader().ReadTitle(title);
    return title;
}

void UnmanagedWrapper::VANGUARD_LOADSTATE(const std::string& file) {
    u64 title_id;
    if (Core::System::GetInstance().GetAppLoader().ReadProgramId(title_id) == Loader::ResultStatus::
        Success) {

        if (!FileUtil::Copy(file, GetSaveStatePath(title_id, 9))) {
            LOG_ERROR(Core, "Could not copy file for loadstate{}", file);
            return;
        }
    }
    Core::System::GetInstance().SendSignal(Core::System::Signal::LoadVanguard, 9);
}

std::string UnmanagedWrapper::VANGUARD_SAVESTATE(const std::string& file) {

    Core::System::GetInstance().SendSignal(Core::System::Signal::Save, 9);

    u64 title_id;
    if (Core::System::GetInstance().GetAppLoader().ReadProgramId(title_id) ==
        Loader::ResultStatus::Success) {

        /*if (!FileUtil::Copy(GetSaveStatePath(title_id, 9), file)) {
            LOG_ERROR(Core, "Could not copy file for savestate{}", file);
            return;
        }*/
        return GetSaveStatePath(title_id, 9);
    }
    return "";
}

void UnmanagedWrapper::VANGUARD_LOADSTATE_DONE() {

    VanguardClientUnmanaged::LOAD_STATE_DONE();
}
void UnmanagedWrapper::VANGUARD_SAVESTATE_DONE() {

    VanguardClientUnmanaged::SAVE_STATE_DONE();
}

void UnmanagedWrapper::PADDR_POKEBYTE(long long addr, unsigned char val, long offset) {
    u8* ptr = Core::System::GetInstance().Memory().GetPhysicalRef(offset).GetPtr();
    unsigned char* dst = ptr + addr;
    std::memcpy(dst, &val, sizeof(u8));
    Core::System::GetInstance().InvalidateCacheRange(reinterpret_cast<u32>(dst), 1);

    if (VideoCore::g_renderer != nullptr) {
        VideoCore::g_renderer->Rasterizer()->InvalidateRegion(reinterpret_cast<PAddr>(dst), 1);
    }

}

unsigned char UnmanagedWrapper::PADDR_PEEKBYTE(long long addr, long offset) {
    auto* ptr = Core::System::GetInstance().Memory().GetPhysicalRef(offset).GetPtr();
    u8 value;
    std::memcpy(&value, ptr + addr, sizeof(u8));
    return value;
}

std::string UnmanagedWrapper::VANGUARD_SAVECONFIG() {
    return "";
}

void UnmanagedWrapper::VANGUARD_LOADCONFIG(std::string cfg) {
}

void UnmanagedWrapper::VANGUARD_CORESTEP() {
    VanguardClientUnmanaged::CORE_STEP();
}

void UnmanagedWrapper::LOAD_STATE_DONE() {

    VanguardClientUnmanaged::LOAD_STATE_DONE();
}
