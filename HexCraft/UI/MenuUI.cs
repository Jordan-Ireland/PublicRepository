using UnityEngine;
using TMPro;
using UnityEngine.UI;
using System;
using System.Collections;

public class MenuUI : MonoBehaviour, IMenu
{
    public GameObject MenuButtons;
    public GameObject OptionsMenu;
    public TMP_Dropdown fullscreenDropdown;
    public TMP_Dropdown graphicsDropdown;
    public Slider mouseXSpeed;
    public Slider mouseYSpeed;

    public void Save()
    {
        global::Save.SaveGame();
        global::Save.SavePlayerSettings();
    }

    public void Open()
    {
        gameObject.SetActive(true);
        UIManager.instance.showMenu = true;
        Reset();
    }

    public void Close()
    {
        gameObject.SetActive(false);
        UIManager.instance.showMenu = false;
        Reset();
    }

    public void ExitApplication()
    {
        Save();
        Application.Quit();
#if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
#endif
    }

    public void ExitMenu()
    {
        Reset();
        UIManager.instance.CloseMenu();
    }

    public void ShowOptions()
    {
        MenuButtons.SetActive(false);
        OptionsMenu.SetActive(true);
        UpdateValues();
    }

    public void Reset()
    {
        MenuButtons.SetActive(true);
        OptionsMenu.SetActive(false);
    }

    void UpdateValues()
    {
        switch (Screen.fullScreenMode)
        {
            case FullScreenMode.ExclusiveFullScreen:
                fullscreenDropdown.value = 1;
                break;
            case FullScreenMode.FullScreenWindow:
                fullscreenDropdown.value = 2;
                break;
            case FullScreenMode.Windowed:
                fullscreenDropdown.value = 0;
                break;
        }
        switch (QualitySettings.GetQualityLevel())
        {
            case 0://low
                graphicsDropdown.value = 0;
                break;
            case 1://medium
                graphicsDropdown.value = 1;
                break;
            case 2://high
                graphicsDropdown.value = 2;
                break;
            case 3://ultra
                graphicsDropdown.value = 3;
                break;
        }

        mouseXSpeed.value = InputManager.MouseXSpeed;
        mouseYSpeed.value = InputManager.MouseYSpeed;
    }
    public void ChangeScreenType()
    {
        switch(fullscreenDropdown.value)
        {
            case 0:
                Screen.fullScreenMode = FullScreenMode.Windowed;
                break;
            case 1:
                Screen.fullScreenMode = FullScreenMode.ExclusiveFullScreen;
                break;
            case 2:
                Screen.fullScreenMode = FullScreenMode.FullScreenWindow;
                break;
        }
    }

    public void ChangeGraphicsType()
    {
        QualitySettings.SetQualityLevel(graphicsDropdown.value);
        Debug.Log(QualitySettings.GetQualityLevel());
    }

    public void SaveOptions()
    {
        ChangeScreenType();
        ChangeGraphicsType();
        InputManager.MouseXSpeed = mouseXSpeed.value;
        InputManager.MouseYSpeed = mouseYSpeed.value;
        //global::Save.SaveOptions();
        global::Save.SavePlayerSettings();
    }

    public void CancelOptions()
    {
        Reset();
    }
}
