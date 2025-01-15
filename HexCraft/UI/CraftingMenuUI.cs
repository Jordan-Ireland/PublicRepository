using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class CraftingMenuUI : MonoBehaviour, IMenu
{
    public enum CraftingTypes { Tools, Food, Length }
    //List View
    [Header("List View")]
    public GameObject ListView;
    public Image selectionIcon;
    public Transform craftingContent;
    public GameObject itemPrefab;
    public CraftingTypes craftingType;
    List<GameObject> itemList = new List<GameObject>();

    //details view
    [Header("Details View")]
    public GameObject DetailsView;
    public TextMeshProUGUI itemName;
    public Image itemIcon;
    public Transform detailsContent;
    public Button craftingButton;
    public GameObject recipeItemPrefab;
    List<GameObject> detailsList = new List<GameObject>();

    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
    }

    public void Open()
    {
        UpdateList();
        gameObject.SetActive(true);
        Reset();
        UIManager.instance.showCraftingMenu = true;
    }

    public void Close()
    {
        Reset();
        UIManager.instance.showCraftingMenu = false;
        gameObject.SetActive(false);
    }

    public void Reset()
    {
        ListView.SetActive(true);
        DetailsView.SetActive(false);
        craftingType = CraftingTypes.Tools;
        selectionIcon.rectTransform.localPosition = new Vector3(0, 115, 0);
        GameManager.instance.recipes = null;
    }

    public void BackButton()
    {
        ListView.SetActive(true);
        DetailsView.SetActive(false);
    }

    public void UpdateList()
    {
        for (int i = 0; i < itemList.Count; i++)
        {
            Destroy(itemList[i]);
        }
        itemList = new List<GameObject>();
        RecipeBook recipes = (GameManager.instance.recipes == null) ? GameManager.instance.standardRecipes : GameManager.instance.recipes;
        for (int i = 0; i < recipes.ids.Count; i++)
        {
            Item item = GameManager.itemDatabase.GetByID(recipes.ids[i]);
            if (SortByType(item))
            {
                GameObject go = Instantiate(itemPrefab, craftingContent);
                go.GetComponent<RectTransform>().anchoredPosition = new Vector3(-10, 135 - (30 * i), 0);
                itemList.Add(go);
                TextMeshProUGUI text = go.GetComponentInChildren<TextMeshProUGUI>();
                Button button = go.GetComponent<Button>();
                button.onClick.AddListener(() => ItemButtonOnClick(item));
                text.SetText(item.Name);
            }
        }
    }

    public void ItemButtonOnClick(Item item)
    {
        for (int i = 0; i < detailsList.Count; i++)
        {
            Destroy(detailsList[i]);
        }
        craftingButton.onClick.RemoveAllListeners();
        detailsList = new List<GameObject>();
        ListView.SetActive(false);
        DetailsView.SetActive(true);
        itemName.SetText(item.Name);
        itemIcon.sprite = item.Icon;
        for (int i = 0; i < item.CraftingRecipe.Count; i++)
        {
            InventoryItem invItem = item.CraftingRecipe[i];
            Item recipeItem = GameManager.itemDatabase.GetByID(invItem.id);
            GameObject go = Instantiate(recipeItemPrefab, detailsContent);
            detailsList.Add(go);
            go.GetComponent<TextMeshProUGUI>().SetText(recipeItem.Name + " x " + invItem.amount);
            //go.GetComponent<RectTransform>().anchoredPosition = new Vector3()
        }
        if (GameManager.playerInstance.inventory.CanCraft(item.CraftingRecipe))
        {
            craftingButton.interactable = true;
            craftingButton.onClick.AddListener(() => CraftItem(item));
        }else
        {
            craftingButton.interactable = false;
        }
    }

    public void CraftItem(Item item)
    {
        GameManager.playerInstance.inventory.AddItemToInventory(item.Id, 1);
        for (int x = 0; x < item.CraftingRecipe.Count; x++)
        {
            GameManager.playerInstance.inventory.RemoveItem(item.CraftingRecipe[x].id, item.CraftingRecipe[x].amount);
        }
        ItemButtonOnClick(item);
    }

    public bool SortByType(Item item)
    {
        if (item.CraftingType == craftingType)
            return true;
        return false;
    }

    public void ChangeCraftingType(int position)
    {
        StartCoroutine(ScrollSelectionIcon(position));
        craftingType = (CraftingTypes)position;
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
}
