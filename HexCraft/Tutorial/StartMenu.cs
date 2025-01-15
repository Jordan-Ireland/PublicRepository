using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using TMPro;

public class StartMenu : MonoBehaviour
{
    public GameObject MenuObject;
    public GameObject LoadingBarObject;
    public GameObject OptionsMenuObject;
    public TMP_Dropdown fullscreenDropdown;
    public TMP_Dropdown graphicsDropdown;
    public TextMeshProUGUI versionText;

    public Slider loadingBarSlider;

    public void Start()
    {
        LoadingBarObject.SetActive(false);
        OptionsMenuObject.SetActive(false);
        Load.LoadSettings(false);
        GridStartMenu.GridInstance.CreateWorld();
        versionText.SetText("Development Build\nVersion " + GameManager.Version);
    }

    public void StartGame()
    {
        // Use a coroutine to load the Scene in the background
        StartCoroutine(LoadGame());
    }

    IEnumerator LoadGame()
    {
        MenuObject.SetActive(false);
        LoadingBarObject.SetActive(true);
        AsyncOperation asyncLoad = SceneManager.LoadSceneAsync(1);
        while(!asyncLoad.isDone)
        {
            loadingBarSlider.value = asyncLoad.progress;
            yield return null;
        }
    }

    public void OpenOptions()
    {
        UpdateValues();
        MenuObject.SetActive(false);
        OptionsMenuObject.SetActive(true);
    }

    public void CloseOptions()
    {
        MenuObject.SetActive(true);
        OptionsMenuObject.SetActive(false);
    }
    void UpdateValues()
    {
        switch (Screen.fullScreenMode)
        {
            case FullScreenMode.Windowed:
                fullscreenDropdown.value = 0;
                break;
            case FullScreenMode.ExclusiveFullScreen:
                fullscreenDropdown.value = 1;
                break;
            case FullScreenMode.FullScreenWindow:
                fullscreenDropdown.value = 2;
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
    }
    public void ChangeScreenType()
    {
        Debug.Log(fullscreenDropdown.value);
        switch (fullscreenDropdown.value)
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
    }

    public void SaveOptions()
    {
        ChangeScreenType();
        ChangeGraphicsType();
        //Save.SaveOptions();
        Save.SavePlayerSettings(false);
        CloseOptions();
    }

    public void Exit()
    {
        Application.Quit();
#if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
#endif
    }
}
