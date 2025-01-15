using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class StartMenu : MonoBehaviour
{
    public GameObject buttons;
    public GameObject controlsMenu;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void StartGame()
    {
        SceneManager.LoadScene(1);
    }

    public void ShowControls()
    {
        buttons.SetActive(false);
        controlsMenu.SetActive(true);
    }

    public void HideControls()
    {
        buttons.SetActive(true);
        controlsMenu.SetActive(false);
    }
}
