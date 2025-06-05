type TField = |{
        type: "text";
        placeholder: string | null;
        label: string | null;
        required: boolean | null;
} |{
};
I would like to be able to represent this in Firebase Data Connect as a union such that I can query it using the following GraphQL query:
JavaScript
   query Fields {
    fields {
      ... on TextField {
        __typename
        id
        placeholder
        label
required }
      ... on SelectField {
        __typename
        id
        placeholder
 type: "select";
placeholder: string | null;
label: string | null;
required: boolean | null;
options: {
  label: string;
}[];
 
  label
        required
        options {
id
label
} }
} }
The fields would then return a union of TextField and SelectField :
JavaScript
{
"data": {
"fields": [ {
           "__typename": "TextField",
           "id": 1,
           "placeholder": "Ex.: Max",
           "label": "First Name",
           "required": false
}, {
] },
         {
           "__typename": "TextField",
           "id": 3,
           "placeholder": null,
           "label": "What is the name of your Dog?",
           "required": true
} ]
} }
 "__typename": "SelectField",
"id": 2,
"placeholder": null,
"label": "Do you like dogs?",
"required": true,
"options": [
  { "id": 1, "label": "Yes!" },
  { "id": 1, "label": "Of course!" }

  So that in my application logic I could do the following:
JavaScript
   data.fields.map((field) => {
     if (field.__typename === "TextField") {
       return <input key={field.id} type="text" />;
     } else if (field.__typename === "SelectField") {
       return (
         <select key={field.id}>
           {field.options.map((option) => (
             <option value={field.id}>{option.label} </option>
           ))}
         </select>
); }
});
