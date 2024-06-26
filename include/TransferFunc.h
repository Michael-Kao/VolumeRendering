#pragma once

#include <vector>
#include <imgui.h>
#include <imgui_internal.h>
#include <iostream>
#include <algorithm>

class TransferFunc {
public:
    TransferFunc();
    void Draw();
    std::vector<float> GetTransferFun();
    bool isChanged;
private:
    std::vector<std::vector<ImVec2>> m_controlPoints;
    int m_currentChannel;
    int m_currentPoint;
    ImVec2 canvas_size;
    ImVec2 canvas_pos;
    bool m_handling;
    void DrawCanvas(ImVec2 size);
    void EventHandling();
    ImVec2 clamp(ImVec2 p, ImVec2 min, ImVec2 max);
    ImVec2 CanvasCoord(ImVec2 p);
};