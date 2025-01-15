using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class HexagonCraftingUI : MonoBehaviour, IMenu
{ 
    public enum Types { Forest, Quarry, Garden, Crafting };
    [Header("List View")]
    public GameObject ListView;
    public Image selectionIcon;
    public Types hexagonType;
    public List<GameObject> itemList = new List<GameObject>();
    public HexagonType hexType;
    public Transform craftingContent;
    public GameObject itemPrefab;
    public Hexagon hexagonPlace;

    [Header("Details View")]
    public GameObject DetailsView;
    public TextMeshProUGUI HexagonName;
    public Transform detailsContent;
    public Button craftingButton;
    public GameObject hexagonItemPrefab;
    List<GameObject> detailsList = new List<GameObject>();

    // Start is called before the first frame update
    void Start()
    {
        UpdateList();
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetKey(InputManager.BuildingCamera) && GameManager.gameState == GameManager.GameState.Building)
        {
            Close();
            GameManager.gameState = GameManager.GameState.MovingToExplore;
        }
    }
    public void UpdateList()
    {
        switch (hexagonType)
        {
            case Types.Forest:
                hexType = GameManager.hexagonDatabase.GetHexagon(HexagonTypes.Forest);
                break;
            case Types.Quarry:
                hexType = GameManager.hexagonDatabase.GetHexagon(HexagonTypes.Quarry);
                break;
            case Types.Crafting:
                hexType = GameManager.hexagonDatabase.GetHexagon(HexagonTypes.Crafting);
                break;
            case Types.Garden:
                hexType = GameManager.hexagonDatabase.GetHexagon(HexagonTypes.Garden);
                break;
        }
        for (int i = 0; i < itemList.Count; i++)
        {
            Destroy(itemList[i]);
        }
        itemList = new List<GameObject>();
        List<HexagonVariation> variations = hexType.hexagonVariations;
        for (int i = 0; i < variations.Count; i++)
        {
            int index = i;
            GameObject go = Instantiate(itemPrefab, craftingContent);
            go.GetComponent<RectTransform>().anchoredPosition = new Vector3(-10, 135 - (30 * i), 0);
            itemList.Add(go);
            TextMeshProUGUI text = go.GetComponentInChildren<TextMeshProUGUI>();
            Button button = go.GetComponent<Button>();
            button.onClick.AddListener(() => ItemButtonOnClick(index));
            text.SetText(hexagonType.ToString() + " - Variation " + (i + 1));
        }
    }

    public void ItemButtonOnClick(int index)
    {
        for (int i = 0; i < detailsList.Count; i++)
        {
            Destroy(detailsList[i]);
        }
        craftingButton.onClick.RemoveAllListeners();
        detailsList = new List<GameObject>();
        ListView.SetActive(false);
        DetailsView.SetActive(true);
        HexagonName.SetText(hexType.hexagonType.ToString() + " - Variation: " + (index + 1));
        for (int i = 0; i < hexType.hexagonVariations[index].resources.Count; i++)
        {
            InventoryItem invItem = hexType.hexagonVariations[index].resources[i];
            Item recipeItem = GameManager.itemDatabase.GetByID(invItem.id);
            GameObject go = Instantiate(hexagonItemPrefab, detailsContent);
            detailsList.Add(go);
            go.GetComponent<TextMeshProUGUI>().SetText(recipeItem.Name + " x " + invItem.amount);
        }
        if (GameManager.playerInstance.inventory.CanCraft(hexType.hexagonVariations[index]))
        {
            craftingButton.interactable = true;
            craftingButton.onClick.AddListener(() => BuildingCamera.CraftHexagon(BuildingCamera.selectedHexagon, hexType, index));
        }
        else
        {
            craftingButton.interactable = false;
        }
    }

    public void ChangeCraftingType(int position)
    {
        StartCoroutine(ScrollSelectionIcon(position));
        hexagonType = (Types)position;
        UpdateList();
    }

    IEnumerator ScrollSelectionIcon(int position)
    {
        Vector3 wantedPosition = new Vector3(0, 115 - (30 * position), 0);
        float dis = Vector3.Distance(selectionIcon.rectTransform.localPosition, wantedPosition);
        while (dis >= 0.5f)
        {
            selectionIcon.rectTransform.localPosition = Vector3.Lerp(selectionIcon.rectTransform.localPosition, wantedPosition, Time.deltaTime * 15);
            dis = Vector3.Distance(selectionIcon.rectTransform.localPosition, wantedPosition);
            yield return new WaitForEndOfFrame();
        }
    }

    public void GoBack()
    {
        ListView.SetActive(true);
        DetailsView.SetActive(false);
    }

    public void Open()
    {
        UIManager.instance.showBuildMenu = true;
        gameObject.SetActive(true);
    }

    public void Close()
    {
        Reset();
        UIManager.instance.showBuildMenu = false;
        gameObject.SetActive(false);
    }

    public void Reset()
    {
        ListView.SetActive(true);
        DetailsView.SetActive(false);
        Destroy(BuildingCamera.temporaryHexagon);
        selectionIcon.rectTransform.localPosition = new Vector3(0, 115, 0);
        hexagonType = Types.Forest;
        UpdateList();
    }
}
