#include "TransferFunc.h"


TransferFunc::TransferFunc()
    : m_controlPoints(std::vector<std::vector<ImVec2>>(4)), m_currentChannel(3), m_currentPoint(0), m_handling(false), isChanged(true)
{
    m_controlPoints[0].push_back(ImVec2(0.0f, 0.0f));
    m_controlPoints[0].push_back(ImVec2(0.83f, 0.89f));
    m_controlPoints[0].push_back(ImVec2(1.0f, 0.2f));

    m_controlPoints[1].push_back(ImVec2(0.0f, 0.0f));
    m_controlPoints[1].push_back(ImVec2(1.0f, 0.5f));

    m_controlPoints[2].push_back(ImVec2(0.0f, 0.0f));
    m_controlPoints[2].push_back(ImVec2(0.21f, 0.98f));
    m_controlPoints[2].push_back(ImVec2(0.78f, 0.12f));
    m_controlPoints[2].push_back(ImVec2(1.0f, 0.8f));

    m_controlPoints[3].push_back(ImVec2(0.0f, 0.0f));
    m_controlPoints[3].push_back(ImVec2(1.0f, 1.0f));
}

void TransferFunc::Draw() {
    // check imgui is initialized
    if (!ImGui::GetCurrentContext()) {
        std::cout << "imgui is not initialized" << std::endl;
        return;
    }
    ImGui::Text("Transfer Function");

    const auto &avail = ImGui::GetContentRegionAvail();
    ImVec2 size = ImVec2(std::min(avail.x, avail.y), std::min(avail.x, avail.y));
    DrawCanvas(size);

    EventHandling();
}

std::vector<float> TransferFunc::GetTransferFun() {
    std::vector<float> res(256 * 4, 0.0f);
    for (std::size_t channel = 0; channel < m_controlPoints.size(); channel++) {
        auto current_control_pt = m_controlPoints[channel].begin();
        for (size_t i = 0; i < 256; i++) {
            float x = static_cast<float>(i) / static_cast<float>(256);
            auto upper = current_control_pt + 1;
            if (x > upper->x) {
                current_control_pt++;
                upper++;
            }

            // Doing lerp
            float t = (x - current_control_pt->x) / (upper->x - current_control_pt->x);
            float value = (1.0f - t) * current_control_pt->y + t * upper->y;
            if(value < 0.0f)    value = 0.0f;
            if(value > 1.0f)    value = 1.0f;
            res[i * 4 + channel] = value;
        }
    }
    return res;
}

void TransferFunc::DrawCanvas(ImVec2 size) {
    const auto& style = ImGui::GetStyle();
    auto* draw_list = ImGui::GetWindowDrawList();

    canvas_pos = ImGui::GetCursorScreenPos();
    canvas_size = ImVec2(size.x - 20, size.y - 20);

    std::vector<ImU32> colors = { 
        IM_COL32(255, 0, 0, 255), 
        IM_COL32(0, 255, 0, 255), 
        IM_COL32(0, 0, 255, 255), 
        IM_COL32(128, 128, 128, 255),
        IM_COL32(255, 255, 255, 255) 
    };

    std::vector<ImU32> darkcolors = { 
        IM_COL32(127, 0, 0, 255), 
        IM_COL32(0, 127, 0, 255), 
        IM_COL32(0, 0, 127, 255), 
        IM_COL32(128, 128, 128, 255),
    };

    ImVec2 minBB = ImVec2(canvas_pos.x, canvas_pos.y);
    ImVec2 maxBB = ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y);
    draw_list->PushClipRect(minBB, maxBB);

    // draw canvas
    ImGui::BeginGroup();
    ImGui::RenderFrame(minBB, maxBB, ImGui::GetColorU32(ImGuiCol_FrameBg, 0.8), true, style.FrameRounding);
    ImGui::InvisibleButton("canvas", ImVec2(maxBB.x - minBB.x, maxBB.y - minBB.y));

    // draw control points
    for(int i = 0; i < 4; ++i) {
        if(i == m_currentChannel)
            continue;

        std::vector<ImVec2>& points = m_controlPoints[i];
        //draw lines
        for(int j = 0; j < points.size() - 1; ++j) {
            ImVec2 p1 = ImVec2(canvas_pos.x + points[j].x * canvas_size.x, canvas_pos.y + (1 - points[j].y) * canvas_size.y);
            ImVec2 p2 = ImVec2(canvas_pos.x + points[j + 1].x * canvas_size.x, canvas_pos.y + (1 - points[j + 1].y) * canvas_size.y);
            draw_list->AddLine(p1, p2, darkcolors[i], 2.0f);
        }

        // draw control points
        for(int j = 0; j < points.size(); ++j) {
            ImVec2 p = ImVec2(canvas_pos.x + points[j].x * canvas_size.x, canvas_pos.y + (1 - points[j].y) * canvas_size.y);
            draw_list->AddCircleFilled(p, 10.0f, darkcolors[i]);
        }
    }

    std::vector<ImVec2>& points = m_controlPoints[m_currentChannel];
    //draw lines
    for(int j = 0; j < points.size() - 1; ++j) {
        ImVec2 p1 = ImVec2(canvas_pos.x + points[j].x * canvas_size.x, canvas_pos.y + (1 - points[j].y) * canvas_size.y);
        ImVec2 p2 = ImVec2(canvas_pos.x + points[j + 1].x * canvas_size.x, canvas_pos.y + (1 - points[j + 1].y) * canvas_size.y);
        draw_list->AddLine(p1, p2, colors[4], 3.0f);
        draw_list->AddLine(p1, p2, colors[m_currentChannel], 2.0f);
    }

    // draw control points
    for(int j = 0; j < points.size(); ++j) {
        ImVec2 p = ImVec2(canvas_pos.x + points[j].x * canvas_size.x, canvas_pos.y + (1 - points[j].y) * canvas_size.y);
        draw_list->AddCircleFilled(p, 12.5f, colors[4]);
        draw_list->AddCircleFilled(p, 10.0f, colors[m_currentChannel]);
    }

    ImGui::EndGroup();

    draw_list->PopClipRect();
}

void TransferFunc::EventHandling() {
    // skip event handling if mouse is not in canvas
    if (!ImGui::IsItemHovered()) {
        return;
    }

    const bool is_left_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    const bool is_right_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Right);
    const bool is_left_released = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    const bool is_right_released = ImGui::IsMouseReleased(ImGuiMouseButton_Right);
    const bool is_left_dragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
    const bool is_clicked = is_left_clicked || is_right_clicked;
    const bool is_released = is_left_released || is_right_released;

    const ImVec2 mouse_position = ImGui::GetIO().MousePos;


    // This radius is based on distance square
    const float reach_radius = 2 * 15 * 15;

    float dist_square_min = std::numeric_limits<float>::max();
    auto near_channel = m_currentChannel;
    auto near_control_pt = m_currentPoint;

    // if(is_clicked) {
    //     std::cout << mouse_position.x << " " << mouse_position.y << std::endl;
    //     std::cout << canvas_pos.x + m_controlPoints[3][0].x * canvas_size.x << " " << canvas_pos.y + (1 - m_controlPoints[3][0].y) * canvas_size.y << std::endl;
    // }

    // The active channel has first priority to respond to events;
    std::vector<ImVec2>& active = m_controlPoints[m_currentChannel];
    for (std::size_t i = 0; i < active.size(); i++) {
        const ImVec2& diff = ImVec2(mouse_position.x - (canvas_pos.x + active[i].x * canvas_size.x), mouse_position.y - (canvas_pos.y + (1 - active[i].y) * canvas_size.y));
        const float dist_square = diff.x * diff.x + diff.y * diff.y;
        if (dist_square < dist_square_min) {
            dist_square_min = dist_square;
            near_control_pt = i;
        }
    }

    // If the closest control point from the current active channel is reachable.
    if (dist_square_min < reach_radius) {
        ImGui::SetTooltip("(%.0f, %.4f)", static_cast<float>(256) * active[near_control_pt].x, active[near_control_pt].y);
        if (is_clicked) {
            // capture the control point
            m_currentPoint = near_control_pt;
            m_handling = true;
        }
    } else {
        // If none of the closest control point from the active channel is reached,
        // We check the other channels
        for (std::size_t i = 0; i < m_controlPoints.size(); i++) {
            if (m_currentChannel == i) {
                continue;
            }
            for (std::size_t j = 0; j < m_controlPoints[i].size(); j++) {
                const ImVec2& diff = ImVec2(mouse_position.x - (canvas_pos.x + m_controlPoints[i][j].x * canvas_size.x), mouse_position.y - (canvas_pos.y + (1 - m_controlPoints[i][j].y) * canvas_size.y));
                const float dist_square = diff.x * diff.x + diff.y * diff.y;
                if (dist_square < dist_square_min) {
                    dist_square_min = dist_square;
                    near_channel = i;
                    near_control_pt = j;
                }
            }
        }

        if (dist_square_min < reach_radius) {
            ImGui::SetTooltip("(%.0f, %.4f)",
                              static_cast<float>(256) * m_controlPoints[near_channel][near_control_pt].x,
                              m_controlPoints[near_channel][near_control_pt].y);
            if (is_clicked) {
                if (near_channel == m_currentChannel) {
                    m_currentPoint = near_control_pt;
                    m_handling = true;
                } else {
                    if (is_left_clicked) {
                        m_currentChannel = near_channel;
                        m_currentPoint = near_control_pt;
                        m_handling = true;
                    }
                }
            }
        }
    }

    if(is_released)
        m_handling = false;

    // Move, Insert, Erase
    if (m_handling) {
        if (is_right_clicked) {
            if (m_currentPoint != 0 && m_currentPoint != active.size() - 1) {
                active.erase(active.begin() + m_currentPoint);
                m_handling = false;
                isChanged = true;
            }
        } else if (is_left_dragging) {
            ImVec2 pos_next = CanvasCoord(mouse_position);
            float x_max = 1.0f;
            float x_min = 0.0f;

            if (m_currentPoint > 0) {
                x_min = active[m_currentPoint - 1].x;
            }
            if (m_currentPoint < active.size() - 1) {
                x_max = active[m_currentPoint + 1].x;
            }

            pos_next = clamp(pos_next, ImVec2(x_min, 0.0f), ImVec2(x_max, 1.0f));

            if (m_currentPoint == 0) {
                pos_next.x = 0;
            }
            if (m_currentPoint == active.size() - 1) {
                pos_next.x = 1;
            }

            active[m_currentPoint] = pos_next;
            isChanged = true;
        }
    } else if (is_right_clicked && active.size() < 20) { //20 is the maximum number of control points
        const auto& mouse_pos = CanvasCoord(mouse_position);
        if (mouse_pos.x > 0.0f && mouse_pos.x < 1.0f && mouse_pos.y > 0.0f && mouse_pos.y < 1.0f) {
            active.emplace_back(mouse_pos);
            std::sort(active.begin(), active.end(), [](const auto& a, const auto& b) { return a.x < b.x; });
        }
        isChanged = true;
    }

    return;
}

ImVec2 TransferFunc::clamp(ImVec2 p, ImVec2 min, ImVec2 max) {
    return ImVec2(std::max(min.x, std::min(p.x, max.x)), std::max(min.y, std::min(p.y, max.y)));

}

ImVec2 TransferFunc::CanvasCoord(ImVec2 p) {
    return ImVec2((p.x - canvas_pos.x) / canvas_size.x, 1 - (p.y - canvas_pos.y) / canvas_size.y);
}