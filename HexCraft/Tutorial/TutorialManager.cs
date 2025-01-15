using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class TutorialManager : MonoBehaviour
{
    public static TutorialManager instance;

    public TutorialSlide[] tutorialSlides;
    TextMeshProUGUI text;

    private void Awake()
    {
        instance = this;
        tutorialSlides = new TutorialSlide[]
        {
            new TutorialSlide("Down on the bottom is your red health points, and in the center is your green experience bar, and in the very middle is your level. Press 'Space Bar'...", KeyCode.Space),
            new TutorialSlide("In the bottom left of your screen is the time of day (daytime or nighttime) and the current day. At night, enemies will spawn. Press 'Space Bar'...", KeyCode.Space),
            new TutorialSlide("Press the '" + InputManager.CraftingMenu.ToString() + "' button to open your crafting menu.", InputManager.CraftingMenu),
            new TutorialSlide("Here you can craft all sorts of tools and food. For more advanced recipes, you may have to find a specialized crafting table. Close the crafting menu with the '" + InputManager.CraftingMenu.ToString() + "'.", InputManager.CraftingMenu),
            new TutorialSlide("Press the '" + InputManager.Inventory.ToString() + "' button to open the inventory.", InputManager.Inventory),
            new TutorialSlide("Here you can view all your items. To hold an item drag and drop it in the bottom 6 item slots. Close the inventory menu with '" + InputManager.Inventory.ToString() + "'.", InputManager.Inventory),
            new TutorialSlide("Open the world overview with '" + InputManager.BuildingCamera.ToString() + "'.", InputManager.BuildingCamera),
            new TutorialSlide("You can move around with 'WASD' and rotate the camera with 'Q' & 'E'. Click on any water (blue) hexagon to craft a new hexagon there! Press 'Space Bar'", KeyCode.Space),
            new TutorialSlide("Close the world overview with '" + InputManager.BuildingCamera.ToString() + "'.", InputManager.BuildingCamera),
            new TutorialSlide("Last, but not least, open the Menu with '" + InputManager.Menu.ToString() + "'.", InputManager.Menu),
            new TutorialSlide("Here you can save, exit, or change settings like mouse speed and graphics. Close the menu with '" + InputManager.Menu.ToString() + "'.", InputManager.Menu),
            new TutorialSlide("You can break down trees, mine minerals, fight enemies, and interact with objects by pressing (or holding) the 'Left Mouse Button'. Press 'Space Bar'...", KeyCode.Space),
            new TutorialSlide("Have fun! Press 'Space Bar'...", KeyCode.Space),
        };
    }

    // Start is called before the first frame update
    void Start()
    {
        if (!GameManager.TutorialComplete)
        {
            text = UIManager.instance.tutorialText;
            StartCoroutine(StartTutorial());
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    IEnumerator StartTutorial()
    {
        int i = 0;
        while(i < tutorialSlides.Length)
        {
            text.SetText(tutorialSlides[i].text);
            if(tutorialSlides[i].anyKey)
            {
                yield return StartCoroutine(WaitForKeyPress());
            }else
            {
                yield return StartCoroutine(WaitForKeyPress(tutorialSlides[i].key));
            }
            i++;
        }
        Debug.Log("Finished tutorial");
        UIManager.instance.TutorialPanel.SetActive(false);
    }

    IEnumerator WaitForKeyPress(KeyCode key)
    {
        yield return new WaitForSeconds(0.25f);
        bool keyPressed = false;
        while(!keyPressed)
        {
            if (Input.GetKey(key))
            {
                keyPressed = true;
            }
            yield return new WaitForEndOfFrame();
        }
    }

    IEnumerator WaitForKeyPress()
    {
        yield return new WaitForSeconds(0.25f);
        bool keyPressed = false;
        while (!keyPressed)
        {
            if(Input.anyKey && !Input.GetMouseButton(1) && !Input.GetMouseButton(2))
            {
                keyPressed = true;
            }
            yield return new WaitForEndOfFrame();
        }
    }
}

public struct TutorialSlide
{
    public string text;
    public KeyCode key;
    public bool anyKey;

    public TutorialSlide(string text, KeyCode key = KeyCode.None, bool anyKey = false)
    {
        this.text = text;
        this.key = key;
        this.anyKey = anyKey;
    }
}
