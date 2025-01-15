using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BuildingCamera : MonoBehaviour
{
    public static BuildingCamera instance;
    public float speed = 15;
    public float yPos = 30;
    public float xRot = 25;
    public Vector3 wantedPos;
    public float yRot;
    public static Hexagon selectedHexagon;
    public static GameObject temporaryHexagon;
    public GameObject hexagonPrefab;
    public LayerMask ignoreLayer;

    // Start is called before the first frame update
    void Start()
    {
        wantedPos = new Vector3(transform.position.x, yPos, transform.position.z);
    }

    private void Update()
    {
        if (GameManager.gameState == GameManager.GameState.Building)
        {
            if (!UIManager.ShowingMenu())
            {
                MouseRay();
            }
            Move();
            Rotate();
        }
        else if (GameManager.gameState == GameManager.GameState.MovingToBuild)
        {
            GameManager.gameState = GameManager.GameState.Building;
        }
        else
        {
            wantedPos = new Vector3(transform.position.x, yPos, transform.position.z);
        }
    }

    // Update is called once per frame
    void FixedUpdate()
    {

    }

    void MouseRay()
    {
        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit hit = new RaycastHit();
        if (Physics.Raycast(ray, out hit, 250, ignoreLayer))
        {
            Hexagon hex = hit.transform.GetComponentInParent<Hexagon>();
            //if (hex != null && Grid.CheckIfConnected((int)hex.myXY.x, (int)hex.myXY.y) && hex.type == Grid.HexagonTypes.Water)
            if (hex != null && !UIManager.instance.IsDeletionMenuOpen())
            {
                if (Input.GetMouseButtonDown(0))
                {
                    selectedHexagon = hex;
                    GameObject go = Instantiate(hexagonPrefab);
                    temporaryHexagon = go;
                    if (hex.type == HexagonTypes.Water)
                    {
                        go.transform.position = Grid.CalcWorldPosNew(hex.myXY);
                        UIManager.instance.ChangeBuildMenu();
                    }
                    else
                    {
                        selectedHexagon = hex.GetComponentInParent<Hexagon>();
                        go.transform.position = selectedHexagon.transform.position + (Vector3.up * 12.5f);
                        UIManager.instance.OpenDeletionMenu();
                    }
                }
            }
        }
    }
    public static void DeleteHexagon()
    {
        Destroy(temporaryHexagon);
        Grid.AddHexagon(HexagonTypes.Water, 0, (int)selectedHexagon.myXY.x, (int)selectedHexagon.myXY.y);
        HexagonType type = GameManager.hexagonDatabase.GetHexagon(selectedHexagon.type);
        for (int i = 0; i < type.hexagonVariations[selectedHexagon.typeIndex].resources.Count; i++)
        {
            GameManager.playerInstance.inventory.AddItemToInventory(type.hexagonVariations[selectedHexagon.typeIndex].resources[i].id,
                                                                    (int)(type.hexagonVariations[selectedHexagon.typeIndex].resources[i].amount / 2));
        }
        //UIManager.instance.ChangeBuildMenu();
    }

    public static void CraftHexagon(Hexagon hex, HexagonType hexType, int index)
    {
        Destroy(temporaryHexagon);
        Grid.AddHexagon(hexType.hexagonType, index, (int)hex.myXY.x, (int)hex.myXY.y);
        GameManager.playerInstance.AddExp(hexType.hexagonVariations[index].expToGive);
        GameManager.playerInstance.inventory.RemoveItems(hexType.hexagonVariations[index]);
        UIManager.instance.ChangeBuildMenu();
    }

    void Move()
    {
        Vector3 input = new Vector3(InputManager.Horizontal, 0, InputManager.Vertical);
        input = transform.TransformDirection(input);
        input *= speed * Time.deltaTime;
        wantedPos += input;

        wantedPos.z = Mathf.Clamp(wantedPos.z, -300, 300);
        wantedPos.x = Mathf.Clamp(wantedPos.x, -300, 300);
        transform.position = Vector3.Lerp(transform.position, new Vector3(wantedPos.x, yPos, wantedPos.z), Time.deltaTime * 10);
        Vector3 rot = new Vector3(xRot, 0, 0);
        transform.rotation = Quaternion.Lerp(transform.rotation, Quaternion.Euler(rot), Time.deltaTime * 10);
    }

    void Rotate()
    {
        yRot += Input.GetAxisRaw("Camera Rotate") * Time.deltaTime * 90;
        transform.rotation = Quaternion.Euler(new Vector3(transform.rotation.eulerAngles.x, yRot, 0));
    }
}
