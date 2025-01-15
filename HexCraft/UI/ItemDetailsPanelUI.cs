using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class ItemDetailsPanelUI : MonoBehaviour
{
    public TextMeshProUGUI nameText;
    RectTransform myRect;
    public Vector3 offset;

    private void Start()
    {
        myRect = GetComponent<RectTransform>();   
    }

    public void MoveToMouse()
    {
        if(myRect == null)
        {
            myRect = GetComponent<RectTransform>();
        }
        //offset.x = Input.mousePosition.x - myRect.anchoredPosition.x;
        //offset.y = Input.mousePosition.y - myRect.anchoredPosition.y;
        myRect.position = Input.mousePosition - offset;
    }
}
