﻿//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "Helpers\StepTimer.h"
#include "Helpers\DeviceResources.h"
#include "Helpers\InputManager.h"
#include "Helpers\SoundPlayer.h"
#include "Helpers\OverlayManager.h"

#include "Content\Sample3DSceneRenderer.h"
#include "Content\SampleDebugTextRenderer.h"
#include "Content\SampleVirtualControllerRenderer.h"


// Renders Direct2D and 3D content on the screen.
namespace DirectXGame1
{
    class DirectXGame1Main : public DX::IDeviceNotify
    {
    public:
        DirectXGame1Main(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~DirectXGame1Main();
        void CreateWindowSizeDependentResources();
        void Update();
        bool Render();

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

    private:
        void InitializeTouchRegions();
        void ProcessInput(std::vector<PlayerInputData>* playerActions);

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        // Note to developer: Replace these with your own content rendering.
        std::unique_ptr<Sample3DSceneRenderer>           m_sceneRenderer;
        std::shared_ptr<SampleDebugTextRenderer>         m_debugTextRenderer;
        std::shared_ptr<SampleVirtualControllerRenderer> m_virtualControllerRenderer;

        // Input, sound, overlay managers
        std::unique_ptr<InputManager>      m_inputManager;
        std::unique_ptr<SoundPlayer>       m_soundPlayer;
        std::shared_ptr<OverlayManager>    m_overlayManager;

        // Rendering loop timer.
        DX::StepTimer m_timer;

        // Tracks which players are connected (0...3).
        unsigned int m_playersConnected;

        // Tracks the touch region ID, allowing you to enable/disable touch regions.
        // Note to developer: Expand this array if you add more touch regions, e.g. for a menu.
        unsigned int m_touchRegionIDs[3];




    };
}